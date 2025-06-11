#include "hzpch.h"
#include "D3D12DescriptorAllocator.h"

namespace Hazel {

    D3D12DescriptorAllocator::D3D12DescriptorAllocator(DescriptorHeapType heapType, uint32_t maxDescriptors)
        : m_HeapType(heapType)
        , m_MaxDescriptors(maxDescriptors)
        , m_CurrentOffset(0)
        , m_DescriptorSize(0)
    {
    }

    D3D12DescriptorAllocator::~D3D12DescriptorAllocator() {
    }

    void D3D12DescriptorAllocator::Initialize(Microsoft::WRL::ComPtr<ID3D12Device> device) {
        m_Device = device;
        
        // 获取描述符大小
        D3D12_DESCRIPTOR_HEAP_TYPE d3d12HeapType = GetD3D12HeapType();
        m_DescriptorSize = m_Device->GetDescriptorHandleIncrementSize(d3d12HeapType);
        
        // 创建描述符堆
        D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
        heapDesc.Type = d3d12HeapType;
        heapDesc.NumDescriptors = m_MaxDescriptors;
        heapDesc.Flags = GetD3D12HeapFlags();
        heapDesc.NodeMask = 0; // Single GPU
        
        HRESULT hr = m_Device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_DescriptorHeap));
        HZ_CORE_ASSERT(SUCCEEDED(hr), "Failed to create D3D12 descriptor heap");
        
        // 初始化空闲块管理
        InitializeFreeBlocks();
        
        HZ_CORE_INFO("D3D12DescriptorAllocator initialized: Type={0}, MaxDescriptors={1}, DescriptorSize={2}", 
                     static_cast<int>(m_HeapType), m_MaxDescriptors, m_DescriptorSize);
    }

    DescriptorAllocation D3D12DescriptorAllocator::Allocate(uint32_t count) {
        std::lock_guard<std::mutex> lock(m_AllocationMutex);
        
        if (count == 0) {
            return DescriptorAllocation{};
        }
        
        // 尝试从空闲块中分配 - 使用 lower_bound 找到第一个足够大的块
        auto it = m_FreeBlocks.lower_bound(count);
        if (it != m_FreeBlocks.end()) {
            uint32_t freeBlockCount = it->first;
            uint32_t freeBlockOffset = it->second;
            
            // 移除这个空闲块
            m_FreeBlocks.erase(it);
            
            // 创建分配结果
            DescriptorAllocation allocation;
            allocation.baseHandle.heapIndex = 0; // 我们只有一个堆
            allocation.baseHandle.cpuHandle = GetCPUHandle(freeBlockOffset).ptr;
            allocation.baseHandle.gpuHandle = GetGPUHandle(freeBlockOffset).ptr;
            allocation.baseHandle.isValid = true;
            allocation.count = count;
            allocation.heapIndex = 0;
            allocation.descriptorSize = m_DescriptorSize;
            
            // 如果空闲块还有剩余，重新插入剩余部分
            if (freeBlockCount > count) {
                uint32_t remainingCount = freeBlockCount - count;
                uint32_t remainingOffset = freeBlockOffset + count;
                m_FreeBlocks.insert({remainingCount, remainingOffset});
            }
            
            return allocation;
        }
        
        // 从连续分配中分配
        if (m_CurrentOffset + count <= m_MaxDescriptors) {
            DescriptorAllocation allocation;
            allocation.baseHandle.heapIndex = 0;
            allocation.baseHandle.cpuHandle = GetCPUHandle(m_CurrentOffset).ptr;
            allocation.baseHandle.gpuHandle = GetGPUHandle(m_CurrentOffset).ptr;
            allocation.baseHandle.isValid = true;
            allocation.count = count;
            allocation.heapIndex = 0;
            allocation.descriptorSize = m_DescriptorSize;
            
            m_CurrentOffset += count;
            return allocation;
        }
        
        // 分配失败
        HZ_CORE_ERROR("D3D12DescriptorAllocator: Out of descriptor space");
        return DescriptorAllocation{};
    }

    void D3D12DescriptorAllocator::Free(const DescriptorAllocation& allocation) {
        if (!allocation.IsValid()) {
            return;
        }
        
        std::lock_guard<std::mutex> lock(m_AllocationMutex);
        
        // 计算偏移量
        D3D12_CPU_DESCRIPTOR_HANDLE heapStart = m_DescriptorHeap->GetCPUDescriptorHandleForHeapStart();
        uint32_t offset = static_cast<uint32_t>((allocation.baseHandle.cpuHandle - heapStart.ptr) / m_DescriptorSize);
        
        // 添加到空闲块 - multimap 会自动按 count 排序
        m_FreeBlocks.insert({allocation.count, offset});
    }

    void D3D12DescriptorAllocator::Reset() {
        std::lock_guard<std::mutex> lock(m_AllocationMutex);
        
        // 清空空闲块映射
        m_FreeBlocks.clear();
        
        // 重置偏移量
        m_CurrentOffset = 0;
        
        // 重新初始化空闲块管理
        InitializeFreeBlocks();
    }

    D3D12_CPU_DESCRIPTOR_HANDLE D3D12DescriptorAllocator::GetCPUHandle(uint32_t index) const {
        D3D12_CPU_DESCRIPTOR_HANDLE handle = m_DescriptorHeap->GetCPUDescriptorHandleForHeapStart();
        handle.ptr += index * m_DescriptorSize;
        return handle;
    }

    D3D12_GPU_DESCRIPTOR_HANDLE D3D12DescriptorAllocator::GetGPUHandle(uint32_t index) const {
        // GPU句柄只有在堆支持GPU访问时才有效
        if (GetD3D12HeapFlags() & D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE) {
            D3D12_GPU_DESCRIPTOR_HANDLE handle = m_DescriptorHeap->GetGPUDescriptorHandleForHeapStart();
            handle.ptr += index * m_DescriptorSize;
            return handle;
        }
        return D3D12_GPU_DESCRIPTOR_HANDLE{ 0 };
    }

    D3D12_DESCRIPTOR_HEAP_TYPE D3D12DescriptorAllocator::GetD3D12HeapType() const {
        switch (m_HeapType) {
            case DescriptorHeapType::CbvSrvUav: return D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
            case DescriptorHeapType::Rtv:       return D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
            case DescriptorHeapType::Dsv:       return D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
            case DescriptorHeapType::Sampler:   return D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
            default:
                HZ_CORE_ASSERT(false, "Unknown descriptor heap type");
                return D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        }
    }

    D3D12_DESCRIPTOR_HEAP_FLAGS D3D12DescriptorAllocator::GetD3D12HeapFlags() const {
        // CBV/SRV/UAV 和 Sampler 堆需要 GPU 访问
        if (m_HeapType == DescriptorHeapType::CbvSrvUav || m_HeapType == DescriptorHeapType::Sampler) {
            return D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        }
        return D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    }

    void D3D12DescriptorAllocator::InitializeFreeBlocks() {
        // 初始状态下没有空闲块，所有分配都从连续分配开始
        // 空闲块将在 Free() 调用时添加
    }

} // namespace Hazel 
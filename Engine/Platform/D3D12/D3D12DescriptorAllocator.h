#pragma once

#include "Hazel/RHI/Interface/IDescritorAllocator.h"
#include "Platform/D3D12/d3dx12.h"
#include "Platform/D3D12/d3dUtil.h"
#include <map>
#include <mutex>

namespace Hazel {

    class D3D12DescriptorAllocator : public IDescriptorAllocator {
    public:
        D3D12DescriptorAllocator(DescriptorHeapType heapType, uint32_t maxDescriptors = 1024);
        virtual ~D3D12DescriptorAllocator();

        // IDescriptorAllocator interface implementation
        virtual DescriptorAllocation Allocate(uint32_t count = 1) override;
        virtual void Free(const DescriptorAllocation& allocation) override;
        virtual void Reset() override;
        virtual DescriptorHeapType GetHeapType() const override { return m_HeapType; }
        virtual uint32_t GetDescriptorSize() const override { return m_DescriptorSize; }

        // D3D12 specific methods
        void Initialize(Microsoft::WRL::ComPtr<ID3D12Device> device);
        ID3D12DescriptorHeap* GetHeap() const { return m_DescriptorHeap.Get(); }
        D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(uint32_t index) const;
        D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle(uint32_t index) const;

    private:
        Microsoft::WRL::ComPtr<ID3D12Device> m_Device;
        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_DescriptorHeap;
        
        DescriptorHeapType m_HeapType;
        uint32_t m_MaxDescriptors;
        uint32_t m_DescriptorSize;
        uint32_t m_CurrentOffset;
        
        // 空闲块管理 - 使用 multimap 按 count 排序，支持相同大小的多个块
        // key: count (块大小), value: offset (块偏移)
        std::multimap<uint32_t, uint32_t> m_FreeBlocks;
        std::mutex m_AllocationMutex;
        
        // Helper functions
        D3D12_DESCRIPTOR_HEAP_TYPE GetD3D12HeapType() const;
        D3D12_DESCRIPTOR_HEAP_FLAGS GetD3D12HeapFlags() const;
        void InitializeFreeBlocks();
    };

} // namespace Hazel 
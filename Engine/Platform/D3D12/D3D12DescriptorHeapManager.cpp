#include "hzpch.h"
#include "D3D12DescriptorHeapManager.h"
#include "Runtime/Graphics/Texture/TextureBuffer.h"
#include "Runtime/Graphics/RHI/Core/Buffer.h"
#include <string>

namespace Hazel {

    D3D12DescriptorHeapManager::D3D12DescriptorHeapManager(Microsoft::WRL::ComPtr<ID3D12Device> device)
        : m_Device(device)
    {
    }

    D3D12DescriptorHeapManager::~D3D12DescriptorHeapManager() {
    }

    void D3D12DescriptorHeapManager::Initialize() {
        // 创建各种类型的分配器
        m_Allocators[DescriptorHeapType::CbvSrvUav] = std::make_unique<D3D12DescriptorAllocator>(DescriptorHeapType::CbvSrvUav, 2048);
        m_Allocators[DescriptorHeapType::Rtv] = std::make_unique<D3D12DescriptorAllocator>(DescriptorHeapType::Rtv, 512);
        m_Allocators[DescriptorHeapType::Dsv] = std::make_unique<D3D12DescriptorAllocator>(DescriptorHeapType::Dsv, 256);
        m_Allocators[DescriptorHeapType::Sampler] = std::make_unique<D3D12DescriptorAllocator>(DescriptorHeapType::Sampler, 256);
        // 添加 ImGui 专用堆 - 主要用于纹理SRV，所以分配适量的大小
        m_Allocators[DescriptorHeapType::ImGuiSrvUav] = std::make_unique<D3D12DescriptorAllocator>(DescriptorHeapType::ImGuiSrvUav, 512);
        
        // 初始化所有分配器
        for (auto& [type, allocator] : m_Allocators) {
            allocator->Initialize(m_Device);
        }
        
        // todo : 主要是为了Imgui，自己有个占位符， 所以这边写死一个占位符。
        auto& allocator = GetAllocator(DescriptorHeapType::ImGuiSrvUav);
        allocator.Allocate(1);

        HZ_CORE_INFO("D3D12DescriptorHeapManager initialized with {0} heap types", m_Allocators.size());
    }

    IDescriptorAllocator& D3D12DescriptorHeapManager::GetAllocator(DescriptorHeapType type) {
        auto it = m_Allocators.find(type);
        HZ_CORE_ASSERT(it != m_Allocators.end(), "Descriptor allocator not found for heap type");
        return *it->second;
    }

    DescriptorAllocation D3D12DescriptorHeapManager::CreateView(DescriptorType type, const void* resourcePtr, const ViewDescription* viewDesc) {
        // 获取对应的堆类型
        DescriptorHeapType heapType = GetHeapTypeForDescriptorType(type);
        
        // 分配描述符
        auto& allocator = GetAllocator(heapType);
        DescriptorAllocation allocation = allocator.Allocate(1);
        
        if (!allocation.IsValid()) {
            HZ_CORE_ERROR("Failed to allocate descriptor for view creation");
            return DescriptorAllocation{};
        }
        
        // 创建视图
        D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle;
        cpuHandle.ptr = allocation.baseHandle.cpuHandle;
        
        CreateViewInternal(type, resourcePtr, cpuHandle, viewDesc);
        
        return allocation;
    }

    void D3D12DescriptorHeapManager::CopyDescriptors(
        uint32_t numDescriptors,
        const DescriptorHandle* srcHandles,
        const DescriptorHandle& dstHandleStart)
    {
        // 准备源句柄数组
        std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> srcCpuHandles(numDescriptors);
        std::vector<UINT> srcRangeSizes(numDescriptors, 1);
        
        for (uint32_t i = 0; i < numDescriptors; ++i) {
            srcCpuHandles[i].ptr = srcHandles[i].cpuHandle;
        }
        
        // 准备目标句柄
        D3D12_CPU_DESCRIPTOR_HANDLE dstCpuHandle;
        dstCpuHandle.ptr = dstHandleStart.cpuHandle;
        
        // 执行复制（假设都是 CBV/SRV/UAV 类型）
        UINT descriptorSize = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
        
        m_Device->CopyDescriptors(
            1, &dstCpuHandle, &numDescriptors,
            numDescriptors, srcCpuHandles.data(), srcRangeSizes.data(),
            D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV
        );
    }

    void* D3D12DescriptorHeapManager::GetHeap(DescriptorHeapType type) const {
        auto it = m_Allocators.find(type);
        if (it != m_Allocators.end()) {
            return it->second->GetHeap();
        }
        return nullptr;
    }

    void D3D12DescriptorHeapManager::CreateViewInternal(DescriptorType type, const void* resourcePtr, const D3D12_CPU_DESCRIPTOR_HANDLE& destHandle, const ViewDescription* viewDesc) {
        switch (type) {
            case DescriptorType::SRV: {
                const TextureBuffer* texture = static_cast<const TextureBuffer*>(resourcePtr);
                ID3D12Resource* d3dResource = static_cast<ID3D12Resource*>(texture->GetNativeResource());
                
                // 设置调试名称 - 包含CPU handle地址
                wchar_t debugName[64];
                swprintf_s(debugName, L"SRV_CPUHandle_%llX", destHandle.ptr);
                d3dResource->SetName(debugName);
                
                D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = ConvertToD3D12SRVDesc(viewDesc);
                m_Device->CreateShaderResourceView(d3dResource, &srvDesc, destHandle);
                
                // 调试日志
                HZ_CORE_INFO("Created SRV - CPU Handle: {0}, Resource: {1}", 
                    destHandle.ptr, reinterpret_cast<uintptr_t>(d3dResource));
                break;
            }
            
            case DescriptorType::RTV: {
                const TextureBuffer* texture = static_cast<const TextureBuffer*>(resourcePtr);
                ID3D12Resource* d3dResource = static_cast<ID3D12Resource*>(texture->GetNativeResource());
                
                // 设置调试名称
                wchar_t debugName[64];
                swprintf_s(debugName, L"RTV_CPUHandle_%llX", destHandle.ptr);
                d3dResource->SetName(debugName);
                
                D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = ConvertToD3D12RTVDesc(viewDesc);
                m_Device->CreateRenderTargetView(d3dResource, &rtvDesc, destHandle);
                
                HZ_CORE_INFO("Created RTV - CPU Handle: {0}, Resource: {1}", 
                    destHandle.ptr, reinterpret_cast<uintptr_t>(d3dResource));
                break;
            }
            
            case DescriptorType::DSV: {
                const TextureBuffer* texture = static_cast<const TextureBuffer*>(resourcePtr);
                ID3D12Resource* d3dResource = static_cast<ID3D12Resource*>(texture->GetNativeResource());
                
                // 设置调试名称
                wchar_t debugName[64];
                swprintf_s(debugName, L"DSV_CPUHandle_%llX", destHandle.ptr);
                d3dResource->SetName(debugName);
                
                D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = ConvertToD3D12DSVDesc(viewDesc);
                m_Device->CreateDepthStencilView(d3dResource, &dsvDesc, destHandle);
                
                HZ_CORE_INFO("Created DSV - CPU Handle: {0}, Resource: {1}", 
                    destHandle.ptr, reinterpret_cast<uintptr_t>(d3dResource));
                break;
            }
            
            case DescriptorType::CBV: {
                const ConstantBuffer* buffer = static_cast<const ConstantBuffer*>(resourcePtr);
                ID3D12Resource* d3dResource = static_cast<ID3D12Resource*>(buffer->GetNativeResource());
                
                // 设置调试名称
                wchar_t debugName[64];
                swprintf_s(debugName, L"CBV_CPUHandle_%llX", destHandle.ptr);
                d3dResource->SetName(debugName);
                
                D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = ConvertToD3D12CBVDesc(viewDesc, d3dResource);
                m_Device->CreateConstantBufferView(&cbvDesc, destHandle);
                
                // CBV特殊调试信息 - 显示GPU虚拟地址
                HZ_CORE_INFO("Created CBV - CPU Handle: {0}, Resource: {1}, GPU VAddr: {2}, Size: {3}", 
                    destHandle.ptr, reinterpret_cast<uintptr_t>(d3dResource), 
                    cbvDesc.BufferLocation, cbvDesc.SizeInBytes);
                break;
            }
            
            default:
                HZ_CORE_ASSERT(false, "Unsupported descriptor type for view creation");
                break;
        }
    }

    DescriptorHeapType D3D12DescriptorHeapManager::GetHeapTypeForDescriptorType(DescriptorType type) const {
        switch (type) {
            case DescriptorType::SRV:
            case DescriptorType::CBV:
            case DescriptorType::UAV:
                return DescriptorHeapType::CbvSrvUav;
            case DescriptorType::RTV:
                return DescriptorHeapType::Rtv;
            case DescriptorType::DSV:
                return DescriptorHeapType::Dsv;
            default:
                HZ_CORE_ASSERT(false, "Unknown descriptor type");
                return DescriptorHeapType::CbvSrvUav;
        }
    }

    // 转换函数实现
    D3D12_SHADER_RESOURCE_VIEW_DESC D3D12DescriptorHeapManager::ConvertToD3D12SRVDesc(const ViewDescription* viewDesc) const {
        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        
        if (viewDesc) {
            srvDesc.Format = ConvertFormat(viewDesc->format);
            srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
            
            switch (viewDesc->dimension) {
                case ViewDescription::ViewDimension::Texture2D:
                    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
                    srvDesc.Texture2D.MipLevels = viewDesc->mipLevels;
                    srvDesc.Texture2D.MostDetailedMip = viewDesc->mostDetailedMip;
                    break;
                case ViewDescription::ViewDimension::Texture3D:
                    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE3D;
                    srvDesc.Texture3D.MipLevels = viewDesc->mipLevels;
                    srvDesc.Texture3D.MostDetailedMip = viewDesc->mostDetailedMip;
                    break;
                case ViewDescription::ViewDimension::TextureCube:
                    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
                    srvDesc.TextureCube.MipLevels = viewDesc->mipLevels;
                    srvDesc.TextureCube.MostDetailedMip = viewDesc->mostDetailedMip;
                    break;
                case ViewDescription::ViewDimension::Buffer:
                    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
                    srvDesc.Buffer.FirstElement = static_cast<UINT64>(viewDesc->bufferOffset / viewDesc->bufferElementSize);
                    srvDesc.Buffer.NumElements = viewDesc->bufferElementCount;
                    break;
                default:
                    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
                    srvDesc.Texture2D.MipLevels = 1;
                    break;
            }
        } else {
            // 默认值
            srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
            srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
            srvDesc.Texture2D.MipLevels = 1;
        }
        
        return srvDesc;
    }

    D3D12_RENDER_TARGET_VIEW_DESC D3D12DescriptorHeapManager::ConvertToD3D12RTVDesc(const ViewDescription* viewDesc) const {
        D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
        
        if (viewDesc) {
            rtvDesc.Format = ConvertFormat(viewDesc->format);
            
            switch (viewDesc->dimension) {
                case ViewDescription::ViewDimension::Texture2D:
                    rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
                    rtvDesc.Texture2D.MipSlice = viewDesc->mostDetailedMip;
                    break;
                case ViewDescription::ViewDimension::Texture3D:
                    rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE3D;
                    rtvDesc.Texture3D.MipSlice = viewDesc->mostDetailedMip;
                    rtvDesc.Texture3D.FirstWSlice = 0;
                    rtvDesc.Texture3D.WSize = 1;
                    break;
                default:
                    rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
                    break;
            }
        } else {
            // 默认值
            rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
        }
        
        return rtvDesc;
    }

    D3D12_DEPTH_STENCIL_VIEW_DESC D3D12DescriptorHeapManager::ConvertToD3D12DSVDesc(const ViewDescription* viewDesc) const {
        D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
        
        if (viewDesc) {
            dsvDesc.Format = ConvertFormat(viewDesc->format);
            
            switch (viewDesc->dimension) {
                case ViewDescription::ViewDimension::Texture2D:
                    dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
                    dsvDesc.Texture2D.MipSlice = viewDesc->mostDetailedMip;
                    break;
                default:
                    dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
                    break;
            }
        } else {
            // 默认值
            dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
            dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
        }
        
        return dsvDesc;
    }

    D3D12_CONSTANT_BUFFER_VIEW_DESC D3D12DescriptorHeapManager::ConvertToD3D12CBVDesc(const ViewDescription* viewDesc, ID3D12Resource* resource) const {
        D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
        
        cbvDesc.BufferLocation = resource->GetGPUVirtualAddress();
        
        if (viewDesc && viewDesc->dimension == ViewDescription::ViewDimension::Buffer) {
            cbvDesc.SizeInBytes = viewDesc->bufferElementCount * viewDesc->bufferElementSize;
            cbvDesc.BufferLocation += viewDesc->bufferOffset;
        } else {
            // 如果没有描述，尝试从资源获取大小
            D3D12_RESOURCE_DESC resourceDesc = resource->GetDesc();
            cbvDesc.SizeInBytes = static_cast<UINT>(resourceDesc.Width);
        }
        
        // CBV size must be 256-byte aligned
        cbvDesc.SizeInBytes = (cbvDesc.SizeInBytes + 255) & ~255;
        
        return cbvDesc;
    }

    DXGI_FORMAT D3D12DescriptorHeapManager::ConvertFormat(ViewDescription::TextureFormat format) const {
        switch (format) {
            case ViewDescription::TextureFormat::RGBA8_UNORM:
                return DXGI_FORMAT_R8G8B8A8_UNORM;
            case ViewDescription::TextureFormat::BGRA8_UNORM:
                return DXGI_FORMAT_B8G8R8A8_UNORM;
            case ViewDescription::TextureFormat::RGBA16_FLOAT:
                return DXGI_FORMAT_R16G16B16A16_FLOAT;
            case ViewDescription::TextureFormat::RGBA32_FLOAT:
                return DXGI_FORMAT_R32G32B32A32_FLOAT;
            case ViewDescription::TextureFormat::D24_UNORM_S8_UINT:
                return DXGI_FORMAT_D24_UNORM_S8_UINT;
            case ViewDescription::TextureFormat::D32_FLOAT:
                return DXGI_FORMAT_D32_FLOAT;
            case ViewDescription::TextureFormat::Unknown:
            default:
                return DXGI_FORMAT_UNKNOWN;
        }
    }

    DescriptorAllocation D3D12DescriptorHeapManager::CreateViewOnHeap(DescriptorType descriptorType, const void* resourcePtr, DescriptorHeapType heapType, const ViewDescription* viewDesc) {
        // 获取指定堆类型的分配器
        auto& allocator = GetAllocator(heapType);
        DescriptorAllocation allocation = allocator.Allocate(1);
        
        if (!allocation.IsValid()) {
            HZ_CORE_ERROR("Failed to allocate descriptor for view creation on specific heap");
            return DescriptorAllocation{};
        }
        
        // 创建视图
        D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle;
        cpuHandle.ptr = allocation.baseHandle.cpuHandle;
        
        CreateViewInternal(descriptorType, resourcePtr, cpuHandle, viewDesc);
        
        return allocation;
    }

} // namespace Hazel 
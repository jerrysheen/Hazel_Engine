#pragma once

#include "Hazel/RHI/Interface/IDescritorHeapManager.h"
#include "D3D12DescriptorAllocator.h"
#include "Platform/D3D12/d3dx12.h"
#include "Platform/D3D12/d3dUtil.h"
#include <unordered_map>
#include <memory>
#include "Hazel/RHI/Interface/DescriptorTypes.h"

namespace Hazel {

    class D3D12DescriptorHeapManager : public IDescriptorHeapManager {
    public:
        D3D12DescriptorHeapManager(Microsoft::WRL::ComPtr<ID3D12Device> device);
        virtual ~D3D12DescriptorHeapManager();

        // IDescriptorHeapManager interface implementation
        virtual void Initialize() override;
        virtual IDescriptorAllocator& GetAllocator(DescriptorHeapType type) override;
        virtual DescriptorAllocation CreateView(DescriptorType type, const void* resourcePtr, const ViewDescription* viewDesc = nullptr) override;
        virtual void CopyDescriptors(
            uint32_t numDescriptors,
            const DescriptorHandle* srcHandles,
            const DescriptorHandle& dstHandleStart) override;
        virtual void* GetHeap(DescriptorHeapType type) const override;

    private:
        Microsoft::WRL::ComPtr<ID3D12Device> m_Device;
        
        // 不同类型的分配器
        std::unordered_map<DescriptorHeapType, std::unique_ptr<D3D12DescriptorAllocator>> m_Allocators;
        
        // Helper functions
        void CreateViewInternal(DescriptorType type, const void* resourcePtr, const D3D12_CPU_DESCRIPTOR_HANDLE& destHandle, const ViewDescription* viewDesc = nullptr);
        DescriptorHeapType GetHeapTypeForDescriptorType(DescriptorType type) const;
        
        // 转换函数：将抽象的ViewDescription转换为D3D12特定的描述符
        D3D12_SHADER_RESOURCE_VIEW_DESC ConvertToD3D12SRVDesc(const ViewDescription* viewDesc) const;
        D3D12_RENDER_TARGET_VIEW_DESC ConvertToD3D12RTVDesc(const ViewDescription* viewDesc) const;
        D3D12_DEPTH_STENCIL_VIEW_DESC ConvertToD3D12DSVDesc(const ViewDescription* viewDesc) const;
        D3D12_CONSTANT_BUFFER_VIEW_DESC ConvertToD3D12CBVDesc(const ViewDescription* viewDesc, ID3D12Resource* resource) const;
        
        // 格式转换
        DXGI_FORMAT ConvertFormat(ViewDescription::TextureFormat format) const;
    };

} // namespace Hazel 
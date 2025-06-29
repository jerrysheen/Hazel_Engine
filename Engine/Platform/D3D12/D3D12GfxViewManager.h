#pragma once

#include "Hazel/RHI/Interface/IGfxViewManager.h"
#include "Hazel/RHI/Interface/DescriptorTypes.h"
#include "Platform/D3D12/d3dx12.h"
#include "Platform/D3D12/d3dUtil.h"
#include <unordered_map>
#include <boost/functional/hash.hpp>

namespace Hazel {

    class D3D12GfxViewManager : public IGfxViewManager {
    public:
        D3D12GfxViewManager();
        virtual ~D3D12GfxViewManager();

        // IGfxViewManager interface implementation
        virtual void Initialize() override;
        
        // Resource view creation - 传入资源对象，内部自动提取UUID
        virtual DescriptorAllocation CreateRenderTargetView(const Ref<TextureBuffer>& texture) override;
        virtual DescriptorAllocation CreateDepthStencilView(const Ref<TextureBuffer>& texture) override;
        virtual DescriptorAllocation CreateShaderResourceView(const Ref<TextureBuffer>& texture) override;
        virtual DescriptorAllocation CreateConstantBufferView(const Ref<ConstantBuffer>& buffer) override;
        
        // Consecutive descriptor allocation and creation
        virtual DescriptorAllocation AllocateDescriptors(uint32_t count, DescriptorHeapType type) override;
        virtual void CreateShaderResourceView(const Ref<TextureBuffer>& texture, const DescriptorAllocation& targetHandle) override;
        virtual void CreateConstantBufferView(const Ref<ConstantBuffer>& buffer, const DescriptorAllocation& targetHandle) override;
        
        // Batch consecutive view creation
        virtual DescriptorAllocation CreateConsecutiveShaderResourceViews(
            const std::vector<Ref<TextureBuffer>>& textures) override;
        
        // Frame management
        virtual void BeginFrame() override;
        virtual void EndFrame() override;
        virtual PerFrameDescriptorAllocator& GetFrameAllocator(DescriptorHeapType type) override;
        
        // Resource lifecycle management
        // 传入ID是因为，避免销毁的时候还引用了Buffer等导致销毁问题。
        virtual void OnResourceDestroyed(const boost::uuids::uuid& resourceId) override;
        virtual DescriptorAllocation GetCachedView(const boost::uuids::uuid& resourceId, DescriptorType type) override;
        virtual void GarbageCollect() override;
        
        // Get heap
        virtual void* GetHeap(DescriptorHeapType type) const override;

        // ImGui 专用接口
        IDescriptorAllocator& GetImGuiAllocator() const;
        void* GetImGuiHeap() const;
        DescriptorAllocation CreateImGuiSRV(const Ref<TextureBuffer>& texture, const ViewDescription* viewDesc = nullptr);

    private:
        Microsoft::WRL::ComPtr<ID3D12Device> m_Device;
        
        // Heap managers for different descriptor types
        std::unique_ptr<IDescriptorHeapManager> m_HeapManager;
        
        // Cached views for resource reuse
        std::unordered_map<boost::uuids::uuid, std::unordered_map<DescriptorType, DescriptorAllocation>, boost::hash<boost::uuids::uuid>> m_ViewCache;
        
        // Frame allocators for temporary descriptors
        std::unordered_map<DescriptorHeapType, std::unique_ptr<PerFrameDescriptorAllocator>> m_FrameAllocators;
        
        // Helper functions
        DescriptorAllocation CreateViewInternal(const void* resource, DescriptorType type, const void* viewDesc = nullptr);
        void InitializeFrameAllocators();
    };

} 
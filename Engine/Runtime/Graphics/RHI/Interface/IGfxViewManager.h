#pragma once

#include "IDescritorHeapManager.h"
#include "PerFrameDescriptorAllocator.h"
#include "DescriptorTypes.h"
#include "IDescritorAllocator.h"
#include <vector>
#include <boost/uuid/uuid.hpp>
#include "Runtime/Graphics/Texture/TextureBuffer.h"
#include "Runtime/Graphics/RHI/Core/Buffer.h"


namespace Hazel {

    class IGfxViewManager {
    public:
        virtual ~IGfxViewManager() = default;
        
        // 初始化
        virtual void Initialize() = 0;
        
        // 资源视图创建 - 传入资源对象，内部自动提取UUID（返回单个描述符的DescriptorAllocation）
        virtual DescriptorAllocation CreateRenderTargetView(const Ref<TextureBuffer>& texture) = 0;
        virtual DescriptorAllocation CreateDepthStencilView(const Ref<TextureBuffer>& texture) = 0;
        virtual DescriptorAllocation CreateShaderResourceView(const Ref<TextureBuffer>& texture) = 0;
        virtual DescriptorAllocation CreateConstantBufferView(const Ref<ConstantBuffer>& buffer) = 0;
        
        // 连续描述符分配和创建
        virtual DescriptorAllocation AllocateDescriptors(uint32_t count, DescriptorHeapType type) = 0;
        virtual void CreateShaderResourceView(const Ref<TextureBuffer>& texture, const DescriptorAllocation& targetAllocation) = 0;
        virtual void CreateConstantBufferView(const Ref<ConstantBuffer>& buffer, const DescriptorAllocation& targetAllocation) = 0;
        
        // 批量连续视图创建
        virtual DescriptorAllocation CreateConsecutiveShaderResourceViews(
            const std::vector<Ref<TextureBuffer>>& textures) = 0;
        
        // 帧管理
        virtual void BeginFrame() = 0;
        virtual void EndFrame() = 0;
        virtual PerFrameDescriptorAllocator& GetFrameAllocator(DescriptorHeapType type) = 0;
        
        // 资源生命周期管理 - 统一使用UUID
        virtual void OnResourceDestroyed(const boost::uuids::uuid& resourceId) = 0;
        virtual DescriptorAllocation GetCachedView(const boost::uuids::uuid& resourceId, DescriptorType type) = 0;
        virtual void GarbageCollect() = 0;
        
        // 获取堆
        virtual void* GetHeap(DescriptorHeapType type) const = 0;
        
        // ImGui 专用接口
        virtual IDescriptorAllocator& GetImGuiAllocator() const = 0;
        virtual void* GetImGuiHeap() const = 0;
        virtual DescriptorAllocation CreateImGuiSRV(const Ref<TextureBuffer>& texture, const ViewDescription* viewDesc = nullptr) = 0;
        
        // 单例访问
        static IGfxViewManager& Get();
    };

}

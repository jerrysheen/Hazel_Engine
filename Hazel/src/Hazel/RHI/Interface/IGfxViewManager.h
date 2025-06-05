#pragma once

#include "IDescritorHeapManager.h"
#include "PerFrameDescriptorAllocator.h"
#include "DescriptorTypes.h"
#include <vector>
#include <boost/uuid/uuid.hpp>

namespace Hazel {

    class TextureBuffer;
    class ConstantBuffer;

    template<typename T>
    using Ref = std::shared_ptr<T>;

    class IGfxViewManager {
    public:
        virtual ~IGfxViewManager() = default;
        
        // 初始化
        virtual void Initialize() = 0;
        
        // 资源视图创建（非连续）
        virtual DescriptorHandle CreateRenderTargetView(const Ref<TextureBuffer>& texture) = 0;
        virtual DescriptorHandle CreateDepthStencilView(const Ref<TextureBuffer>& texture) = 0;
        virtual DescriptorHandle CreateShaderResourceView(const Ref<TextureBuffer>& texture) = 0;
        virtual DescriptorHandle CreateConstantBufferView(const Ref<ConstantBuffer>& buffer) = 0;
        
        // 连续描述符分配和创建
        virtual DescriptorAllocation AllocateDescriptors(uint32_t count, DescriptorHeapType type) = 0;
        virtual void CreateShaderResourceView(const Ref<TextureBuffer>& texture, const DescriptorHandle& targetHandle) = 0;
        virtual void CreateConstantBufferView(const Ref<ConstantBuffer>& buffer, const DescriptorHandle& targetHandle) = 0;
        
        // 批量连续视图创建
        virtual DescriptorAllocation CreateConsecutiveShaderResourceViews(
            const std::vector<Ref<TextureBuffer>>& textures) = 0;
        
        // 帧管理
        virtual void BeginFrame() = 0;
        virtual void EndFrame() = 0;
        virtual PerFrameDescriptorAllocator& GetFrameAllocator(DescriptorHeapType type) = 0;
        
        // 资源生命周期管理
        virtual void OnResourceDestroyed(const boost::uuids::uuid& resourceId) = 0;
        virtual DescriptorHandle GetCachedView(const boost::uuids::uuid& resourceId, DescriptorType type) = 0;
        virtual void GarbageCollect() = 0;
        
        // 获取堆
        virtual void* GetHeap(DescriptorHeapType type) const = 0;
        
        // 单例访问
        static IGfxViewManager& Get();
    };

}

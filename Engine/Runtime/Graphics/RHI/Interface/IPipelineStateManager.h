#pragma once

#include "IGraphicsPipeline.h"
#include "PipelineTypes.h"
#include <unordered_map>

namespace Hazel {

    class IPipelineStateManager {
    public:
        virtual ~IPipelineStateManager() = default;
        
        // 创建图形管线
        virtual Ref<IGraphicsPipeline> CreateGraphicsPipeline(const GraphicsPipelineDesc& desc) = 0;
        
        // 管线缓存和复用
        virtual Ref<IGraphicsPipeline> GetOrCreatePipeline(const GraphicsPipelineDesc& desc) = 0;
        
        // 清理无用的管线
        virtual void GarbageCollect() = 0;
        
        // 获取缓存统计信息
        virtual size_t GetCachedPipelineCount() const = 0;
        
        // 单例访问（类似IGfxViewManager::Get()的风格）
        static IPipelineStateManager& Get();
        
        // 单例清理（用于程序退出时的资源清理）
        static void Shutdown();
        
    protected:
        // 管线哈希和缓存
        virtual uint64_t HashPipelineDesc(const GraphicsPipelineDesc& desc) const = 0;
        std::unordered_map<uint64_t, std::weak_ptr<IGraphicsPipeline>> m_PipelineCache;
    };

} // namespace Hazel 
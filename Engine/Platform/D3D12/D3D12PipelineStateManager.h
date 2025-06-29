#pragma once

#include "Hazel/RHI/Interface/IPipelineStateManager.h"
#include "D3D12GraphicsPipeline.h"
#include "Platform/D3D12/d3dx12.h"
#include "Platform/D3D12/d3dUtil.h"
#include <functional>

namespace Hazel {

    class D3D12PipelineStateManager : public IPipelineStateManager {
    public:
        D3D12PipelineStateManager();
        virtual ~D3D12PipelineStateManager();
        
        // IPipelineStateManager 实现
        virtual Ref<IGraphicsPipeline> CreateGraphicsPipeline(const GraphicsPipelineDesc& desc) override;
        virtual Ref<IGraphicsPipeline> GetOrCreatePipeline(const GraphicsPipelineDesc& desc) override;
        virtual void GarbageCollect() override;
        virtual size_t GetCachedPipelineCount() const override;
        
    protected:
        virtual uint64_t HashPipelineDesc(const GraphicsPipelineDesc& desc) const override;
        
    private:
        // 辅助哈希方法
        size_t HashRasterizerState(const RasterizerStateDesc& desc) const;
        size_t HashBlendState(const BlendStateDesc& desc) const;
        size_t HashDepthStencilState(const DepthStencilStateDesc& desc) const;
        // 初始化D3D12设备引用
        void Initialize();
        
        // D3D12设备引用
        Microsoft::WRL::ComPtr<ID3D12Device> m_Device;
        
        // 管线描述哈希器
        struct PipelineDescHasher {
            size_t operator()(const GraphicsPipelineDesc& desc) const;
        };
        
        // 管线描述比较器
        struct PipelineDescComparer {
            bool operator()(const GraphicsPipelineDesc& lhs, const GraphicsPipelineDesc& rhs) const;
        };
        
        // 私有构造函数，支持单例模式
        friend class IPipelineStateManager;
        
        // 垃圾回收的策略设置
        static constexpr size_t MAX_CACHED_PIPELINES = 1000;
        static constexpr size_t GC_THRESHOLD = 800;
    };

} // namespace Hazel 
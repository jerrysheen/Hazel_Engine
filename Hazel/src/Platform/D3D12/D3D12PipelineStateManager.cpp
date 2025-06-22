#include "hzpch.h"
#include "D3D12PipelineStateManager.h"
#include "D3D12RenderAPIManager.h"
#include <functional>

namespace Hazel {

    D3D12PipelineStateManager::D3D12PipelineStateManager()
    {
        Initialize();
    }

    D3D12PipelineStateManager::~D3D12PipelineStateManager()
    {
        // 清理所有缓存的管线
        m_PipelineCache.clear();
    }

    void D3D12PipelineStateManager::Initialize()
    {
        // 获取D3D12设备
        D3D12RenderAPIManager* renderAPIManager = dynamic_cast<D3D12RenderAPIManager*>(
            RenderAPIManager::getInstance()->GetManager().get());
        m_Device = renderAPIManager->GetD3DDevice();
        HZ_CORE_ASSERT(m_Device, "Failed to get D3D12 device");
    }

    Ref<IGraphicsPipeline> D3D12PipelineStateManager::CreateGraphicsPipeline(const GraphicsPipelineDesc& desc)
    {
        // 直接创建新的管线，不使用缓存
        return CreateRef<D3D12GraphicsPipeline>(desc);
    }

    Ref<IGraphicsPipeline> D3D12PipelineStateManager::GetOrCreatePipeline(const GraphicsPipelineDesc& desc)
    {
        // 计算管线描述的哈希值
        uint64_t hash = HashPipelineDesc(desc);
        
        // 检查缓存中是否存在
        auto it = m_PipelineCache.find(hash);
        if (it != m_PipelineCache.end()) {
            // 检查缓存的管线是否仍然有效
            if (auto pipeline = it->second.lock()) {
                return pipeline;
            } else {
                // 弱引用已失效，从缓存中移除
                m_PipelineCache.erase(it);
            }
        }
        
        // 创建新的管线
        auto pipeline = CreateRef<D3D12GraphicsPipeline>(desc);
        
        // 添加到缓存（使用弱引用避免循环引用）
        m_PipelineCache[hash] = std::weak_ptr<IGraphicsPipeline>(pipeline);
        
        // 检查是否需要垃圾回收
        if (m_PipelineCache.size() > GC_THRESHOLD) {
            GarbageCollect();
        }
        
        return pipeline;
    }

    void D3D12PipelineStateManager::GarbageCollect()
    {
        // 移除所有无效的弱引用
        auto it = m_PipelineCache.begin();
        while (it != m_PipelineCache.end()) {
            if (it->second.expired()) {
                it = m_PipelineCache.erase(it);
            } else {
                ++it;
            }
        }
        
        // 如果缓存仍然过大，进行更激进的清理
        if (m_PipelineCache.size() > MAX_CACHED_PIPELINES) {
            HZ_CORE_WARN("Pipeline cache size exceeded limit, clearing cache");
            m_PipelineCache.clear();
        }
    }

    size_t D3D12PipelineStateManager::GetCachedPipelineCount() const
    {
        return m_PipelineCache.size();
    }

    uint64_t D3D12PipelineStateManager::HashPipelineDesc(const GraphicsPipelineDesc& desc) const
    {
        // 使用std::hash组合各个组件的哈希值
        size_t hash = 0;
        
        // 哈希shader指针（假设同一个shader对象表示同一个shader）
        hash ^= std::hash<void*>{}(desc.shader.get()) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        
        // 哈希光栅化状态
        hash ^= HashRasterizerState(desc.rasterizerState) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        
        // 哈希混合状态
        hash ^= HashBlendState(desc.blendState) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        
        // 哈希深度模板状态
        hash ^= HashDepthStencilState(desc.depthStencilState) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        
        // 哈希图元拓扑
        hash ^= std::hash<int>{}(static_cast<int>(desc.primitiveTopology)) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        
        // 哈希渲染目标格式
        hash ^= std::hash<int>{}(static_cast<int>(desc.colorFormat)) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        hash ^= std::hash<int>{}(static_cast<int>(desc.depthStencilFormat)) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        
        // 哈希采样设置
        hash ^= std::hash<uint32_t>{}(desc.sampleCount) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        hash ^= std::hash<uint32_t>{}(desc.sampleQuality) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        
        return hash;
    }

    size_t D3D12PipelineStateManager::HashRasterizerState(const RasterizerStateDesc& desc) const
    {
        size_t hash = 0;
        hash ^= std::hash<int>{}(static_cast<int>(desc.fillMode));
        hash ^= std::hash<int>{}(static_cast<int>(desc.cullMode)) << 1;
        hash ^= std::hash<bool>{}(desc.frontCounterClockwise) << 2;
        hash ^= std::hash<bool>{}(desc.depthClipEnable) << 3;
        hash ^= std::hash<bool>{}(desc.scissorEnable) << 4;
        hash ^= std::hash<float>{}(desc.depthBias) << 5;
        hash ^= std::hash<float>{}(desc.depthBiasClamp) << 6;
        hash ^= std::hash<float>{}(desc.slopeScaledDepthBias) << 7;
        return hash;
    }

    size_t D3D12PipelineStateManager::HashBlendState(const BlendStateDesc& desc) const
    {
        size_t hash = 0;
        hash ^= std::hash<bool>{}(desc.alphaToCoverageEnable);
        hash ^= std::hash<bool>{}(desc.independentBlendEnable) << 1;
        
        // 对于简化，只哈希第一个渲染目标的混合状态
        const auto& rt = desc.renderTargetBlend[0];
        hash ^= std::hash<bool>{}(rt.blendEnable) << 2;
        hash ^= std::hash<int>{}(static_cast<int>(rt.srcColorBlendFactor)) << 3;
        hash ^= std::hash<int>{}(static_cast<int>(rt.dstColorBlendFactor)) << 4;
        hash ^= std::hash<int>{}(static_cast<int>(rt.colorBlendOp)) << 5;
        hash ^= std::hash<int>{}(static_cast<int>(rt.srcAlphaBlendFactor)) << 6;
        hash ^= std::hash<int>{}(static_cast<int>(rt.dstAlphaBlendFactor)) << 7;
        hash ^= std::hash<int>{}(static_cast<int>(rt.alphaBlendOp)) << 8;
        hash ^= std::hash<uint8_t>{}(rt.colorWriteMask) << 9;
        
        return hash;
    }

    size_t D3D12PipelineStateManager::HashDepthStencilState(const DepthStencilStateDesc& desc) const
    {
        size_t hash = 0;
        hash ^= std::hash<bool>{}(desc.depthEnable);
        hash ^= std::hash<bool>{}(desc.depthWriteEnable) << 1;
        hash ^= std::hash<int>{}(static_cast<int>(desc.depthFunc)) << 2;
        hash ^= std::hash<bool>{}(desc.stencilEnable) << 3;
        hash ^= std::hash<uint8_t>{}(desc.stencilReadMask) << 4;
        hash ^= std::hash<uint8_t>{}(desc.stencilWriteMask) << 5;
        return hash;
    }

    size_t D3D12PipelineStateManager::PipelineDescHasher::operator()(const GraphicsPipelineDesc& desc) const
    {
        // 这是一个静态哈希器，但我们使用实例方法来复用代码
        // 在实际使用中，这个哈希器可能不会被用到，因为我们使用uint64_t作为键
        return 0; // 占位符实现
    }

    bool D3D12PipelineStateManager::PipelineDescComparer::operator()(const GraphicsPipelineDesc& lhs, const GraphicsPipelineDesc& rhs) const
    {
        // 比较两个管线描述是否相等
        if (lhs.shader != rhs.shader) return false;
        if (lhs.primitiveTopology != rhs.primitiveTopology) return false;
        if (lhs.colorFormat != rhs.colorFormat) return false;
        if (lhs.depthStencilFormat != rhs.depthStencilFormat) return false;
        if (lhs.sampleCount != rhs.sampleCount) return false;
        if (lhs.sampleQuality != rhs.sampleQuality) return false;
        
        // 比较光栅化状态
        const auto& lRaster = lhs.rasterizerState;
        const auto& rRaster = rhs.rasterizerState;
        if (std::memcmp(&lRaster, &rRaster, sizeof(RasterizerStateDesc)) != 0) return false;
        
        // 比较混合状态
        const auto& lBlend = lhs.blendState;
        const auto& rBlend = rhs.blendState;
        if (std::memcmp(&lBlend, &rBlend, sizeof(BlendStateDesc)) != 0) return false;
        
        // 比较深度模板状态
        const auto& lDepth = lhs.depthStencilState;
        const auto& rDepth = rhs.depthStencilState;
        if (std::memcmp(&lDepth, &rDepth, sizeof(DepthStencilStateDesc)) != 0) return false;
        
        return true;
    }

} // namespace Hazel 
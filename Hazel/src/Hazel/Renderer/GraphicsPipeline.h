#pragma once

//#include "Hazel/Core/Base.h"
#include "Hazel/Renderer/Shader.h"
#include "Hazel/Renderer/Buffer.h"
#include "Hazel/Renderer/TextureStruct.h"

namespace Hazel {

    // 混合因子
    enum class BlendFactor {
        Zero,
        One,
        SrcColor,
        InvSrcColor,
        SrcAlpha,
        InvSrcAlpha,
        DstAlpha,
        InvDstAlpha,
        DstColor,
        InvDstColor,
        SrcAlphaSat,
        BlendFactor,
        InvBlendFactor
    };

    // 混合操作
    enum class BlendOp {
        Add,
        Subtract,
        RevSubtract,
        Min,
        Max
    };

    // 混合模式
    enum class BlendMode {
        None,
        Alpha,
        Additive,
        Multiply,
        Custom
    };

    // 深度测试函数
    enum class DepthFunc {
        Never,
        Less,
        LessEqual,
        Equal,
        NotEqual,
        GreaterEqual,
        Greater,
        Always
    };

    // 表面剔除模式
    enum class CullMode {
        None,
        Front,
        Back
    };

    // 多边形填充模式
    enum class FillMode {
        Solid,
        Wireframe,
        Point
    };

    // 原语拓扑类型
    enum class PrimitiveTopology {
        PointList,
        LineList,
        LineStrip,
        TriangleList,
        TriangleStrip
    };

    // 光栅化状态描述
    struct RasterizerState {
        FillMode fillMode = FillMode::Solid;
        CullMode cullMode = CullMode::Back;
        bool frontCounterClockwise = false;
        bool depthClipEnable = true;
        bool scissorEnable = false;
        bool multisampleEnable = false;
        bool antialiasedLineEnable = false;
        float depthBias = 0.0f;
        float depthBiasClamp = 0.0f;
        float slopeScaledDepthBias = 0.0f;
    };

    // 混合状态描述
    struct BlendState {
        bool alphaToCoverageEnable = false;
        bool independentBlendEnable = false;

        struct RenderTargetBlend {
            bool blendEnable = false;
            BlendMode blendMode = BlendMode::Alpha;
            // 如果需要自定义混合模式，以下属性将被使用
            // 在简单使用时，可以通过blendMode快速设置常用预设
            BlendFactor srcColorBlendFactor = BlendFactor::One;
            BlendFactor dstColorBlendFactor = BlendFactor::Zero;
            BlendOp colorBlendOp = BlendOp::Add;
            BlendFactor srcAlphaBlendFactor = BlendFactor::One;
            BlendFactor dstAlphaBlendFactor = BlendFactor::Zero;
            BlendOp alphaBlendOp = BlendOp::Add;
            uint8_t colorWriteMask = 0xF; // RGBA全部启用
        };

        RenderTargetBlend renderTargetBlend[8]; // 支持多个渲染目标
    };

    // 深度模板状态描述
    struct DepthStencilState {
        bool depthEnable = true;
        bool depthWriteEnable = true;
        DepthFunc depthFunc = DepthFunc::Less;
        bool stencilEnable = false;
        uint8_t stencilReadMask = 0xFF;
        uint8_t stencilWriteMask = 0xFF;
        
        // 更多模板测试相关配置...
    };

    // 管线状态描述
    struct GraphicsPipelineDesc {
        Ref<Shader> shader;
        Ref<BufferLayout> layout;
        RasterizerState rasterizerState;
        BlendState blendState;
        DepthStencilState depthStencilState;
        PrimitiveTopology primitiveTopology = PrimitiveTopology::TriangleList;
        TextureFormat colorFormat = TextureFormat::RGBA32;
        TextureFormat depthStencilFormat = TextureFormat::DEPTH24STENCIL8;
        uint32_t sampleCount = 1;
        uint32_t sampleQuality = 0;
    };

    class GraphicsPipeline {
    public:
        virtual ~GraphicsPipeline() = default;

        // 创建一个图形管线
        static Ref<GraphicsPipeline> Create(const GraphicsPipelineDesc& desc);

        // 绑定管线以进行渲染
        virtual void Bind() const = 0;
        
        // 获取管线描述
        const GraphicsPipelineDesc& GetDescription() const { return m_Description; }

    protected:
        GraphicsPipelineDesc m_Description;
    };

} // namespace Hazel

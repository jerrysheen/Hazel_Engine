#pragma once

#include "Hazel/Core/Core.h"

namespace Hazel {

    // 前向声明
    class Shader;

    // 基础渲染状态枚举
    enum class BlendFactor {
        Zero, One, SrcColor, InvSrcColor, SrcAlpha, InvSrcAlpha,
        DstColor, InvDstColor, DstAlpha, InvDstAlpha, ConstantColor, InvConstantColor
    };

    enum class BlendOp {
        Add, Subtract, ReverseSubtract, Min, Max
    };

    enum class DepthFunc {
        Never, Less, LessEqual, Equal, NotEqual, GreaterEqual, Greater, Always
    };

    enum class CullMode {
        None, Front, Back
    };

    enum class FillMode {
        Solid, Wireframe, Point
    };

    enum class PrimitiveTopology {
        PointList, LineList, LineStrip, TriangleList, TriangleStrip
    };

    // 管线状态描述结构体
    struct RasterizerStateDesc {
        FillMode fillMode = FillMode::Solid;
        CullMode cullMode = CullMode::Back;
        bool frontCounterClockwise = false;
        bool depthClipEnable = true;
        bool scissorEnable = false;
        float depthBias = 0.0f;
        float depthBiasClamp = 0.0f;
        float slopeScaledDepthBias = 0.0f;
        
        // 便捷静态方法
        static RasterizerStateDesc Default() {
            return RasterizerStateDesc{};
        }
        
        static RasterizerStateDesc NoCull() {
            RasterizerStateDesc desc;
            desc.cullMode = CullMode::None;
            return desc;
        }
        
        static RasterizerStateDesc Wireframe() {
            RasterizerStateDesc desc;
            desc.fillMode = FillMode::Wireframe;
            return desc;
        }
    };

    struct BlendStateDesc {
        struct RenderTargetBlend {
            bool blendEnable = false;
            BlendFactor srcColorBlendFactor = BlendFactor::One;
            BlendFactor dstColorBlendFactor = BlendFactor::Zero;
            BlendOp colorBlendOp = BlendOp::Add;
            BlendFactor srcAlphaBlendFactor = BlendFactor::One;
            BlendFactor dstAlphaBlendFactor = BlendFactor::Zero;
            BlendOp alphaBlendOp = BlendOp::Add;
            uint8_t colorWriteMask = 0xF; // RGBA
        };

        bool alphaToCoverageEnable = false;
        bool independentBlendEnable = false;
        RenderTargetBlend renderTargetBlend[8];
        
        // 便捷静态方法
        static BlendStateDesc Opaque() {
            return BlendStateDesc{};
        }
        
        static BlendStateDesc AlphaBlend() {
            BlendStateDesc desc;
            desc.renderTargetBlend[0].blendEnable = true;
            desc.renderTargetBlend[0].srcColorBlendFactor = BlendFactor::SrcAlpha;
            desc.renderTargetBlend[0].dstColorBlendFactor = BlendFactor::InvSrcAlpha;
            return desc;
        }
        
        static BlendStateDesc Additive() {
            BlendStateDesc desc;
            desc.renderTargetBlend[0].blendEnable = true;
            desc.renderTargetBlend[0].srcColorBlendFactor = BlendFactor::One;
            desc.renderTargetBlend[0].dstColorBlendFactor = BlendFactor::One;
            return desc;
        }
    };

    struct DepthStencilStateDesc {
        bool depthEnable = true;
        bool depthWriteEnable = true;
        DepthFunc depthFunc = DepthFunc::Less;
        bool stencilEnable = false;
        uint8_t stencilReadMask = 0xFF;
        uint8_t stencilWriteMask = 0xFF;
        
        // 便捷静态方法
        static DepthStencilStateDesc Default() {
            return DepthStencilStateDesc{};
        }
        
        static DepthStencilStateDesc ReadOnly() {
            DepthStencilStateDesc desc;
            desc.depthWriteEnable = false;
            return desc;
        }
        
        static DepthStencilStateDesc Disabled() {
            DepthStencilStateDesc desc;
            desc.depthEnable = false;
            desc.depthWriteEnable = false;
            return desc;
        }
    };

    // 图形管线描述结构（专注于渲染状态）
    struct GraphicsPipelineDesc {
        Ref<Shader> shader;                    // Shader程序
        RasterizerStateDesc rasterizerState;   // 光栅化状态
        BlendStateDesc blendState;             // 混合状态
        DepthStencilStateDesc depthStencilState; // 深度模板状态
        PrimitiveTopology primitiveTopology = PrimitiveTopology::TriangleList;
        
        // 渲染目标格式
        enum class TextureFormat {
            RGBA8, BGRA8, RGBA16F, RGBA32F, DEPTH24STENCIL8, DEPTH32F
        } colorFormat = TextureFormat::RGBA8;
        TextureFormat depthStencilFormat = TextureFormat::DEPTH24STENCIL8;
        
        uint32_t sampleCount = 1;
        uint32_t sampleQuality = 0;
        
        // 便捷链式设置方法
        GraphicsPipelineDesc& SetShader(const Ref<Shader>& shader_) {
            shader = shader_;
            return *this;
        }
        
        GraphicsPipelineDesc& SetRasterizerState(const RasterizerStateDesc& state) {
            rasterizerState = state;
            return *this;
        }
        
        GraphicsPipelineDesc& SetBlendState(const BlendStateDesc& state) {
            blendState = state;
            return *this;
        }
        
        GraphicsPipelineDesc& SetDepthStencilState(const DepthStencilStateDesc& state) {
            depthStencilState = state;
            return *this;
        }
        
        GraphicsPipelineDesc& SetPrimitiveTopology(PrimitiveTopology topology) {
            primitiveTopology = topology;
            return *this;
        }
    };

    // 管线状态句柄 (类似DescriptorHandle的设计)
    struct PipelineStateHandle {
        uint64_t id = 0;
        bool isValid = false;
        
        bool IsValid() const { return isValid; }
    };

} // namespace Hazel 
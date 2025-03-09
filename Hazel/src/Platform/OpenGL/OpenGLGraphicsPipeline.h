#pragma once

#include "Hazel/Renderer/GraphicsPipeline.h"

namespace Hazel {

    class OpenGLGraphicsPipeline : public GraphicsPipeline {
    public:
        OpenGLGraphicsPipeline(const GraphicsPipelineDesc& desc);
        virtual ~OpenGLGraphicsPipeline();

        virtual void Bind() const override;
        
        // OpenGL特定的辅助方法
        void EnableDepthTest(bool enable) const;
        void EnableBlending(bool enable) const;
        void EnableCulling(bool enable) const;
        void EnableWireframe(bool enable) const;

    private:
        // 初始化各种状态
        void InitializeRasterizerState();
        void InitializeBlendState();
        void InitializeDepthStencilState();
        void InitializeProgram();

        // 工具函数，将Hazel枚举转换为OpenGL枚举
        uint32_t TranslateCullMode(CullMode mode) const;
        uint32_t TranslateFillMode(FillMode mode) const;
        uint32_t TranslateDepthFunc(DepthFunc func) const;
        uint32_t TranslateBlendFactor(BlendFactor factor) const;
        uint32_t TranslateBlendOp(BlendOp op) const;
        uint32_t TranslatePrimitiveTopology(PrimitiveTopology topology) const;
        void ConfigureBlendStateForBlendMode(BlendMode mode);

        // 虽然OpenGL没有显式的PSO对象，我们在对象中缓存各种状态，在Bind()调用时应用
        // 顶点数组对象
        uint32_t m_VertexArrayObject = 0;
        
        // 着色器程序
        uint32_t m_ShaderProgram = 0;
        
        // 缓存状态配置
        struct {
            bool depthTest = true;
            bool depthWrite = true;
            uint32_t depthFunc = 0; // GL_LESS
            
            bool stencilTest = false;
            uint32_t stencilMask = 0xFF;
            
            bool blend = false;
            uint32_t srcColorBlendFactor = 0; // GL_ONE
            uint32_t dstColorBlendFactor = 0; // GL_ZERO
            uint32_t colorBlendOp = 0;        // GL_FUNC_ADD
            uint32_t srcAlphaBlendFactor = 0;
            uint32_t dstAlphaBlendFactor = 0;
            uint32_t alphaBlendOp = 0;
            
            bool cullFace = true;
            uint32_t cullMode = 0; // GL_BACK
            bool frontCounterClockwise = false;
            
            uint32_t polygonMode = 0; // GL_FILL
            uint32_t primitiveTopology = 0; // GL_TRIANGLES
        } m_State;
    };

}

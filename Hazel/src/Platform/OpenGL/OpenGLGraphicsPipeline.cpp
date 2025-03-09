#include "hzpch.h"
#include "OpenGLGraphicsPipeline.h"
#include "OpenGLShader.h"
#include "OpenGLContext.h"
#include "glad/glad.h"

namespace Hazel {

    OpenGLGraphicsPipeline::OpenGLGraphicsPipeline(const GraphicsPipelineDesc& desc)
    {
       // HZ_PROFILE_FUNCTION();

        // 保存描述信息到基类
        m_Description = desc;

        // 初始化状态
        InitializeProgram();
        InitializeRasterizerState();
        InitializeBlendState();
        InitializeDepthStencilState();
    }

    OpenGLGraphicsPipeline::~OpenGLGraphicsPipeline()
    {
        // 删除顶点数组对象（如果有）
        if (m_VertexArrayObject != 0)
            glDeleteVertexArrays(1, &m_VertexArrayObject);
    }

    void OpenGLGraphicsPipeline::Bind() const
    {
        //HZ_PROFILE_FUNCTION();

        // 绑定着色器程序
        if (m_ShaderProgram != 0)
            glUseProgram(m_ShaderProgram);
        
        // 绑定顶点数组对象
        if (m_VertexArrayObject != 0)
            glBindVertexArray(m_VertexArrayObject);

        // 应用深度测试状态
        if (m_State.depthTest)
        {
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(m_State.depthFunc);
        }
        else
        {
            glDisable(GL_DEPTH_TEST);
        }

        // 应用深度写入
        glDepthMask(m_State.depthWrite ? GL_TRUE : GL_FALSE);

        // 应用模板测试状态
        if (m_State.stencilTest)
        {
            glEnable(GL_STENCIL_TEST);
            glStencilMask(m_State.stencilMask);
        }
        else
        {
            glDisable(GL_STENCIL_TEST);
        }

        // 应用混合状态
        if (m_State.blend)
        {
            glEnable(GL_BLEND);
            glBlendEquationSeparate(m_State.colorBlendOp, m_State.alphaBlendOp);
            glBlendFuncSeparate(
                m_State.srcColorBlendFactor, m_State.dstColorBlendFactor,
                m_State.srcAlphaBlendFactor, m_State.dstAlphaBlendFactor
            );
        }
        else
        {
            glDisable(GL_BLEND);
        }

        // 应用剔除状态
        if (m_State.cullFace)
        {
            glEnable(GL_CULL_FACE);
            glCullFace(m_State.cullMode);
            glFrontFace(m_State.frontCounterClockwise ? GL_CCW : GL_CW);
        }
        else
        {
            glDisable(GL_CULL_FACE);
        }

        // 应用填充模式
        glPolygonMode(GL_FRONT_AND_BACK, m_State.polygonMode);
    }

    void OpenGLGraphicsPipeline::InitializeProgram()
    {
        //HZ_PROFILE_FUNCTION();

        // 从描述中获取着色器
        if (m_Description.shader)
        {
            auto openglShader = std::dynamic_pointer_cast<OpenGLShader>(m_Description.shader);
            if (openglShader)
            {
                //m_ShaderProgram = openglShader->GetRendererID();
            }
        }

        // 创建顶点数组对象
        glCreateVertexArrays(1, &m_VertexArrayObject);

        // 如果提供了布局，设置顶点属性
        if (m_Description.layout)
        {
            const auto& layout = *m_Description.layout;
            
            // 绑定VAO以配置它
            glBindVertexArray(m_VertexArrayObject);
            
            // 设置顶点属性
            uint32_t index = 0;
            for (const auto& element : layout.GetElements())
            {
                // 启用顶点属性
                glEnableVertexAttribArray(index);
                
                switch (element.Type)
                {
                    case ShaderDataType::Float:
                    case ShaderDataType::Float2:
                    case ShaderDataType::Float3:
                    case ShaderDataType::Float4:
                    {
                        glVertexAttribPointer(
                            index,
                            element.GetComponentCount(),
                            GL_FLOAT,
                            element.Normalized ? GL_TRUE : GL_FALSE,
                            layout.GetStride(),
                            (const void*)(intptr_t)element.Offset
                        );
                        break;
                    }
                    case ShaderDataType::Int:
                    case ShaderDataType::Int2:
                    case ShaderDataType::Int3:
                    case ShaderDataType::Int4:
                    {
                        glVertexAttribIPointer(
                            index,
                            element.GetComponentCount(),
                            GL_INT,
                            layout.GetStride(),
                            (const void*)(intptr_t)element.Offset
                        );
                        break;
                    }
                    case ShaderDataType::Bool:
                    {
                        glVertexAttribIPointer(
                            index,
                            1,
                            GL_BOOL,
                            layout.GetStride(),
                            (const void*)(intptr_t)element.Offset
                        );
                        break;
                    }
                    // 其他类型...
                }
                
                index++;
            }
            
            // 解绑VAO
            glBindVertexArray(0);
        }
    }

    void OpenGLGraphicsPipeline::InitializeRasterizerState()
    {
        //HZ_PROFILE_FUNCTION();

        // 设置剔除模式
        const auto& rasterizer = m_Description.rasterizerState;
        
        m_State.cullFace = (rasterizer.cullMode != CullMode::None);
        m_State.cullMode = TranslateCullMode(rasterizer.cullMode);
        m_State.frontCounterClockwise = rasterizer.frontCounterClockwise;
        
        // 设置填充模式
        m_State.polygonMode = TranslateFillMode(rasterizer.fillMode);
        
        // 设置图元拓扑
        m_State.primitiveTopology = TranslatePrimitiveTopology(m_Description.primitiveTopology);
    }

    void OpenGLGraphicsPipeline::InitializeBlendState()
    {
        //HZ_PROFILE_FUNCTION();

        const auto& blendState = m_Description.blendState;
        const auto& rtBlend = blendState.renderTargetBlend[0]; // 仅使用第一个渲染目标的配置

        // 设置是否启用混合
        m_State.blend = rtBlend.blendEnable;
        
        if (rtBlend.blendEnable)
        {
            if (rtBlend.blendMode != BlendMode::Custom)
            {
                // 使用预设混合模式
                ConfigureBlendStateForBlendMode(rtBlend.blendMode);
            }
            else
            {
                // 使用自定义混合因子和操作
                m_State.srcColorBlendFactor = TranslateBlendFactor(rtBlend.srcColorBlendFactor);
                m_State.dstColorBlendFactor = TranslateBlendFactor(rtBlend.dstColorBlendFactor);
                m_State.colorBlendOp = TranslateBlendOp(rtBlend.colorBlendOp);
                m_State.srcAlphaBlendFactor = TranslateBlendFactor(rtBlend.srcAlphaBlendFactor);
                m_State.dstAlphaBlendFactor = TranslateBlendFactor(rtBlend.dstAlphaBlendFactor);
                m_State.alphaBlendOp = TranslateBlendOp(rtBlend.alphaBlendOp);
            }
        }
    }

    void OpenGLGraphicsPipeline::InitializeDepthStencilState()
    {
        //HZ_PROFILE_FUNCTION();

        const auto& depthStencil = m_Description.depthStencilState;
        
        // 设置深度测试状态
        m_State.depthTest = depthStencil.depthEnable;
        m_State.depthWrite = depthStencil.depthWriteEnable;
        m_State.depthFunc = TranslateDepthFunc(depthStencil.depthFunc);
        
        // 设置模板测试状态
        m_State.stencilTest = depthStencil.stencilEnable;
        m_State.stencilMask = depthStencil.stencilWriteMask;
    }

    void OpenGLGraphicsPipeline::EnableDepthTest(bool enable) const
    {
        if (enable)
            glEnable(GL_DEPTH_TEST);
        else
            glDisable(GL_DEPTH_TEST);
    }

    void OpenGLGraphicsPipeline::EnableBlending(bool enable) const
    {
        if (enable)
            glEnable(GL_BLEND);
        else
            glDisable(GL_BLEND);
    }

    void OpenGLGraphicsPipeline::EnableCulling(bool enable) const
    {
        if (enable)
            glEnable(GL_CULL_FACE);
        else
            glDisable(GL_CULL_FACE);
    }

    void OpenGLGraphicsPipeline::EnableWireframe(bool enable) const
    {
        glPolygonMode(GL_FRONT_AND_BACK, enable ? GL_LINE : GL_FILL);
    }

    uint32_t OpenGLGraphicsPipeline::TranslateCullMode(CullMode mode) const
    {
        switch (mode)
        {
            case CullMode::None:  return 0; // 特殊情况，不使用
            case CullMode::Front: return GL_FRONT;
            case CullMode::Back:  return GL_BACK;
            default:              return GL_BACK;
        }
    }

    uint32_t OpenGLGraphicsPipeline::TranslateFillMode(FillMode mode) const
    {
        switch (mode)
        {
            case FillMode::Point:     return GL_POINT;
            case FillMode::Wireframe: return GL_LINE;
            case FillMode::Solid:     return GL_FILL;
            default:                  return GL_FILL;
        }
    }

    uint32_t OpenGLGraphicsPipeline::TranslateDepthFunc(DepthFunc func) const
    {
        switch (func)
        {
            case DepthFunc::Never:        return GL_NEVER;
            case DepthFunc::Less:         return GL_LESS;
            case DepthFunc::LessEqual:    return GL_LEQUAL;
            case DepthFunc::Equal:        return GL_EQUAL;
            case DepthFunc::NotEqual:     return GL_NOTEQUAL;
            case DepthFunc::GreaterEqual: return GL_GEQUAL;
            case DepthFunc::Greater:      return GL_GREATER;
            case DepthFunc::Always:       return GL_ALWAYS;
            default:                      return GL_LESS;
        }
    }

    uint32_t OpenGLGraphicsPipeline::TranslateBlendFactor(BlendFactor factor) const
    {
        switch (factor)
        {
            case BlendFactor::Zero:           return GL_ZERO;
            case BlendFactor::One:            return GL_ONE;
            case BlendFactor::SrcColor:       return GL_SRC_COLOR;
            case BlendFactor::InvSrcColor:    return GL_ONE_MINUS_SRC_COLOR;
            case BlendFactor::SrcAlpha:       return GL_SRC_ALPHA;
            case BlendFactor::InvSrcAlpha:    return GL_ONE_MINUS_SRC_ALPHA;
            case BlendFactor::DstAlpha:       return GL_DST_ALPHA;
            case BlendFactor::InvDstAlpha:    return GL_ONE_MINUS_DST_ALPHA;
            case BlendFactor::DstColor:       return GL_DST_COLOR;
            case BlendFactor::InvDstColor:    return GL_ONE_MINUS_DST_COLOR;
            case BlendFactor::SrcAlphaSat:    return GL_SRC_ALPHA_SATURATE;
            case BlendFactor::BlendFactor:    return GL_CONSTANT_COLOR;
            case BlendFactor::InvBlendFactor: return GL_ONE_MINUS_CONSTANT_COLOR;
            default:                          return GL_ONE;
        }
    }

    uint32_t OpenGLGraphicsPipeline::TranslateBlendOp(BlendOp op) const
    {
        switch (op)
        {
            case BlendOp::Add:         return GL_FUNC_ADD;
            case BlendOp::Subtract:    return GL_FUNC_SUBTRACT;
            case BlendOp::RevSubtract: return GL_FUNC_REVERSE_SUBTRACT;
            case BlendOp::Min:         return GL_MIN;
            case BlendOp::Max:         return GL_MAX;
            default:                   return GL_FUNC_ADD;
        }
    }

    uint32_t OpenGLGraphicsPipeline::TranslatePrimitiveTopology(PrimitiveTopology topology) const
    {
        switch (topology)
        {
            case PrimitiveTopology::PointList:     return GL_POINTS;
            case PrimitiveTopology::LineList:      return GL_LINES;
            case PrimitiveTopology::LineStrip:     return GL_LINE_STRIP;
            case PrimitiveTopology::TriangleList:  return GL_TRIANGLES;
            case PrimitiveTopology::TriangleStrip: return GL_TRIANGLE_STRIP;
            default:                               return GL_TRIANGLES;
        }
    }

    void OpenGLGraphicsPipeline::ConfigureBlendStateForBlendMode(BlendMode mode)
    {
        switch (mode)
        {
            case BlendMode::Alpha:
                m_State.srcColorBlendFactor = GL_SRC_ALPHA;
                m_State.dstColorBlendFactor = GL_ONE_MINUS_SRC_ALPHA;
                m_State.colorBlendOp = GL_FUNC_ADD;
                m_State.srcAlphaBlendFactor = GL_ONE;
                m_State.dstAlphaBlendFactor = GL_ONE_MINUS_SRC_ALPHA;
                m_State.alphaBlendOp = GL_FUNC_ADD;
                break;
                
            case BlendMode::Additive:
                m_State.srcColorBlendFactor = GL_SRC_ALPHA;
                m_State.dstColorBlendFactor = GL_ONE;
                m_State.colorBlendOp = GL_FUNC_ADD;
                m_State.srcAlphaBlendFactor = GL_ONE;
                m_State.dstAlphaBlendFactor = GL_ONE;
                m_State.alphaBlendOp = GL_FUNC_ADD;
                break;
                
            case BlendMode::Multiply:
                m_State.srcColorBlendFactor = GL_DST_COLOR;
                m_State.dstColorBlendFactor = GL_ZERO;
                m_State.colorBlendOp = GL_FUNC_ADD;
                m_State.srcAlphaBlendFactor = GL_DST_ALPHA;
                m_State.dstAlphaBlendFactor = GL_ZERO;
                m_State.alphaBlendOp = GL_FUNC_ADD;
                break;
                
            case BlendMode::None:
            default:
                m_State.srcColorBlendFactor = GL_ONE;
                m_State.dstColorBlendFactor = GL_ZERO;
                m_State.colorBlendOp = GL_FUNC_ADD;
                m_State.srcAlphaBlendFactor = GL_ONE;
                m_State.dstAlphaBlendFactor = GL_ZERO;
                m_State.alphaBlendOp = GL_FUNC_ADD;
                break;
        }
    }

}

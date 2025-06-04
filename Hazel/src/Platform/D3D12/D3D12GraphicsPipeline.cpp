#include "hzpch.h"
//#include "D3D12GraphicsPipeline.h"
//#include "D3D12Context.h"
//#include "D3D12Shader.h"
//
//namespace Hazel {
//
//    D3D12GraphicsPipeline::D3D12GraphicsPipeline(const GraphicsPipelineDesc& desc)
//        : m_Description(desc), m_Device(nullptr)
//    {
//        HZ_PROFILE_FUNCTION();
//
//        // 获取D3D12设备
//        m_Device = D3D12Context::GetDevice();
//        HZ_CORE_ASSERT(m_Device, "D3D12设备未初始化！");
//
//        // 创建根签名和管线状态对象
//        CreateRootSignature();
//        CreatePipelineState();
//    }
//
//    D3D12GraphicsPipeline::~D3D12GraphicsPipeline()
//    {
//        // COM智能指针会自动释放资源
//    }
//
//    void D3D12GraphicsPipeline::Bind() const
//    {
//        HZ_PROFILE_FUNCTION();
//
//        auto commandList = D3D12Context::GetCommandList();
//        
//        // 设置根签名
//        commandList->SetGraphicsRootSignature(m_RootSignature.Get());
//        
//        // 设置管线状态
//        commandList->SetPipelineState(m_PipelineState.Get());
//
//        // 设置图元拓扑
//        D3D12_PRIMITIVE_TOPOLOGY topology;
//        switch (m_Description.primitiveTopology)
//        {
//            case PrimitiveTopology::PointList:     topology = D3D_PRIMITIVE_TOPOLOGY_POINTLIST; break;
//            case PrimitiveTopology::LineList:      topology = D3D_PRIMITIVE_TOPOLOGY_LINELIST; break;
//            case PrimitiveTopology::LineStrip:     topology = D3D_PRIMITIVE_TOPOLOGY_LINESTRIP; break;
//            case PrimitiveTopology::TriangleList:  topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST; break;
//            case PrimitiveTopology::TriangleStrip: topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP; break;
//            default: topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
//        }
//        commandList->IASetPrimitiveTopology(topology);
//    }
//
//    void D3D12GraphicsPipeline::CreateRootSignature()
//    {
//        HZ_PROFILE_FUNCTION();
//
//        // 获取着色器中的反射信息用于创建根签名
//        auto d3d12Shader = std::dynamic_pointer_cast<D3D12Shader>(m_Description.shader);
//        HZ_CORE_ASSERT(d3d12Shader, "无效的着色器!");
//
//        // 从着色器中获取或创建根签名
//        m_RootSignature = d3d12Shader->GetRootSignature();
//        
//        // 如果着色器没有内置根签名，创建一个基本的根签名
//        if (!m_RootSignature)
//        {
//            CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
//            rootSignatureDesc.Init(0, nullptr, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
//
//            Microsoft::WRL::ComPtr<ID3DBlob> signature;
//            Microsoft::WRL::ComPtr<ID3DBlob> error;
//            
//            HRESULT hr = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error);
//            
//            if (FAILED(hr))
//            {
//                if (error)
//                {
//                    const char* errorMsg = static_cast<const char*>(error->GetBufferPointer());
//                    HZ_CORE_ERROR("根签名序列化失败: {0}", errorMsg);
//                }
//                HZ_CORE_ASSERT(false, "根签名序列化失败!");
//            }
//
//            hr = m_Device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), 
//                                              IID_PPV_ARGS(&m_RootSignature));
//            HZ_CORE_ASSERT(SUCCEEDED(hr), "根签名创建失败!");
//        }
//    }
//
//    void D3D12GraphicsPipeline::CreatePipelineState()
//    {
//        HZ_PROFILE_FUNCTION();
//
//        // 获取着色器字节码
//        auto d3d12Shader = std::dynamic_pointer_cast<D3D12Shader>(m_Description.shader);
//        HZ_CORE_ASSERT(d3d12Shader, "无效的着色器!");
//
//        D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
//        
//        // 设置根签名
//        psoDesc.pRootSignature = m_RootSignature.Get();
//        
//        // 设置着色器程序
//        auto shaderBytecode = d3d12Shader->GetShaderBytecode();
//        psoDesc.VS = shaderBytecode.VS;
//        psoDesc.PS = shaderBytecode.PS;
//        psoDesc.DS = shaderBytecode.DS;
//        psoDesc.HS = shaderBytecode.HS;
//        psoDesc.GS = shaderBytecode.GS;
//        
//        // 设置光栅化状态
//        psoDesc.RasterizerState.FillMode = TranslateFillMode(m_Description.rasterizerState.fillMode);
//        psoDesc.RasterizerState.CullMode = TranslateCullMode(m_Description.rasterizerState.cullMode);
//        psoDesc.RasterizerState.FrontCounterClockwise = m_Description.rasterizerState.frontCounterClockwise;
//        psoDesc.RasterizerState.DepthBias = static_cast<INT>(m_Description.rasterizerState.depthBias);
//        psoDesc.RasterizerState.DepthBiasClamp = m_Description.rasterizerState.depthBiasClamp;
//        psoDesc.RasterizerState.SlopeScaledDepthBias = m_Description.rasterizerState.slopeScaledDepthBias;
//        psoDesc.RasterizerState.DepthClipEnable = m_Description.rasterizerState.depthClipEnable;
//        psoDesc.RasterizerState.MultisampleEnable = m_Description.rasterizerState.multisampleEnable;
//        psoDesc.RasterizerState.AntialiasedLineEnable = m_Description.rasterizerState.antialiasedLineEnable;
//        psoDesc.RasterizerState.ForcedSampleCount = 0;
//        psoDesc.RasterizerState.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
//        
//        // 设置混合状态
//        psoDesc.BlendState.AlphaToCoverageEnable = m_Description.blendState.alphaToCoverageEnable;
//        psoDesc.BlendState.IndependentBlendEnable = m_Description.blendState.independentBlendEnable;
//        
//        for (uint32_t i = 0; i < 8; i++)
//        {
//            const auto& srcBlend = m_Description.blendState.renderTargetBlend[i];
//            auto& dstBlend = psoDesc.BlendState.RenderTarget[i];
//            
//            dstBlend.BlendEnable = srcBlend.blendEnable;
//            dstBlend.LogicOpEnable = FALSE;
//            dstBlend.LogicOp = D3D12_LOGIC_OP_NOOP;
//            
//            if (srcBlend.blendEnable)
//            {
//                // 如果不是自定义混合模式，使用预设
//                if (srcBlend.blendMode != BlendMode::Custom)
//                {
//                    ConfigureBlendStateForBlendMode(srcBlend.blendMode, dstBlend);
//                }
//                else
//                {
//                    // 使用自定义混合模式
//                    dstBlend.SrcBlend = TranslateBlendFactor(srcBlend.srcColorBlendFactor);
//                    dstBlend.DestBlend = TranslateBlendFactor(srcBlend.dstColorBlendFactor);
//                    dstBlend.BlendOp = TranslateBlendOp(srcBlend.colorBlendOp);
//                    dstBlend.SrcBlendAlpha = TranslateBlendFactor(srcBlend.srcAlphaBlendFactor);
//                    dstBlend.DestBlendAlpha = TranslateBlendFactor(srcBlend.dstAlphaBlendFactor);
//                    dstBlend.BlendOpAlpha = TranslateBlendOp(srcBlend.alphaBlendOp);
//                }
//            }
//            
//            dstBlend.RenderTargetWriteMask = srcBlend.colorWriteMask;
//        }
//        
//        // 设置深度/模板状态
//        psoDesc.DepthStencilState.DepthEnable = m_Description.depthStencilState.depthEnable;
//        psoDesc.DepthStencilState.DepthWriteMask = m_Description.depthStencilState.depthWriteEnable ? 
//            D3D12_DEPTH_WRITE_MASK_ALL : D3D12_DEPTH_WRITE_MASK_ZERO;
//        psoDesc.DepthStencilState.DepthFunc = TranslateDepthFunc(m_Description.depthStencilState.depthFunc);
//        psoDesc.DepthStencilState.StencilEnable = m_Description.depthStencilState.stencilEnable;
//        psoDesc.DepthStencilState.StencilReadMask = m_Description.depthStencilState.stencilReadMask;
//        psoDesc.DepthStencilState.StencilWriteMask = m_Description.depthStencilState.stencilWriteMask;
//        
//        // 此处简化了模板操作，实际应用中可能需要更详细的配置
//        psoDesc.DepthStencilState.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
//        psoDesc.DepthStencilState.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
//        psoDesc.DepthStencilState.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
//        psoDesc.DepthStencilState.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
//        psoDesc.DepthStencilState.BackFace = psoDesc.DepthStencilState.FrontFace;
//        
//        // 设置输入布局
//        if (m_Description.layout)
//        {
//            // 此处需要将Hazel的顶点布局转换为D3D12的输入布局
//            // 假设存在一个转换函数或D3D12VertexBufferLayout实现了GetD3D12InputLayout
//            std::vector<D3D12_INPUT_ELEMENT_DESC> inputLayout;
//            // 这里需要实现从BufferLayout到D3D12_INPUT_ELEMENT_DESC的转换
//            // TODO: 实现顶点布局转换
//            
//            psoDesc.InputLayout.NumElements = static_cast<UINT>(inputLayout.size());
//            psoDesc.InputLayout.pInputElementDescs = inputLayout.data();
//        }
//        
//        // 设置图元拓扑类型
//        psoDesc.PrimitiveTopologyType = TranslatePrimitiveTopology(m_Description.primitiveTopology);
//        
//        // 设置渲染目标格式
//        psoDesc.NumRenderTargets = 1;
//        psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM; // 应根据colorFormat转换
//        psoDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT; // 应根据depthStencilFormat转换
//        
//        // 设置采样
//        psoDesc.SampleMask = UINT_MAX;
//        psoDesc.SampleDesc.Count = m_Description.sampleCount;
//        psoDesc.SampleDesc.Quality = m_Description.sampleQuality;
//        
//        // 创建管线状态对象
//        HRESULT hr = m_Device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_PipelineState));
//        if (FAILED(hr))
//        {
//            HZ_CORE_ERROR("创建图形管线状态对象失败，HRESULT: 0x{0:X}", static_cast<uint32_t>(hr));
//            HZ_CORE_ASSERT(false, "管线状态对象创建失败!");
//        }
//    }
//
//    D3D12_CULL_MODE D3D12GraphicsPipeline::TranslateCullMode(CullMode mode) const
//    {
//        switch (mode)
//        {
//            case CullMode::None:  return D3D12_CULL_MODE_NONE;
//            case CullMode::Front: return D3D12_CULL_MODE_FRONT;
//            case CullMode::Back:  return D3D12_CULL_MODE_BACK;
//            default:              return D3D12_CULL_MODE_BACK;
//        }
//    }
//
//    D3D12_FILL_MODE D3D12GraphicsPipeline::TranslateFillMode(FillMode mode) const
//    {
//        switch (mode)
//        {
//            case FillMode::Wireframe: return D3D12_FILL_MODE_WIREFRAME;
//            case FillMode::Solid:     return D3D12_FILL_MODE_SOLID;
//            default:                  return D3D12_FILL_MODE_SOLID;
//        }
//    }
//
//    D3D12_PRIMITIVE_TOPOLOGY_TYPE D3D12GraphicsPipeline::TranslatePrimitiveTopology(PrimitiveTopology topology) const
//    {
//        switch (topology)
//        {
//            case PrimitiveTopology::PointList:     return D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
//            case PrimitiveTopology::LineList:
//            case PrimitiveTopology::LineStrip:     return D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
//            case PrimitiveTopology::TriangleList:
//            case PrimitiveTopology::TriangleStrip: return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
//            default:                               return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
//        }
//    }
//
//    D3D12_COMPARISON_FUNC D3D12GraphicsPipeline::TranslateDepthFunc(DepthFunc func) const
//    {
//        switch (func)
//        {
//            case DepthFunc::Never:        return D3D12_COMPARISON_FUNC_NEVER;
//            case DepthFunc::Less:         return D3D12_COMPARISON_FUNC_LESS;
//            case DepthFunc::LessEqual:    return D3D12_COMPARISON_FUNC_LESS_EQUAL;
//            case DepthFunc::Equal:        return D3D12_COMPARISON_FUNC_EQUAL;
//            case DepthFunc::NotEqual:     return D3D12_COMPARISON_FUNC_NOT_EQUAL;
//            case DepthFunc::GreaterEqual: return D3D12_COMPARISON_FUNC_GREATER_EQUAL;
//            case DepthFunc::Greater:      return D3D12_COMPARISON_FUNC_GREATER;
//            case DepthFunc::Always:       return D3D12_COMPARISON_FUNC_ALWAYS;
//            default:                      return D3D12_COMPARISON_FUNC_LESS;
//        }
//    }
//
//    D3D12_BLEND D3D12GraphicsPipeline::TranslateBlendFactor(BlendFactor factor) const
//    {
//        switch (factor)
//        {
//            case BlendFactor::Zero:          return D3D12_BLEND_ZERO;
//            case BlendFactor::One:           return D3D12_BLEND_ONE;
//            case BlendFactor::SrcColor:      return D3D12_BLEND_SRC_COLOR;
//            case BlendFactor::InvSrcColor:   return D3D12_BLEND_INV_SRC_COLOR;
//            case BlendFactor::SrcAlpha:      return D3D12_BLEND_SRC_ALPHA;
//            case BlendFactor::InvSrcAlpha:   return D3D12_BLEND_INV_SRC_ALPHA;
//            case BlendFactor::DstAlpha:      return D3D12_BLEND_DEST_ALPHA;
//            case BlendFactor::InvDstAlpha:   return D3D12_BLEND_INV_DEST_ALPHA;
//            case BlendFactor::DstColor:      return D3D12_BLEND_DEST_COLOR;
//            case BlendFactor::InvDstColor:   return D3D12_BLEND_INV_DEST_COLOR;
//            case BlendFactor::SrcAlphaSat:   return D3D12_BLEND_SRC_ALPHA_SAT;
//            case BlendFactor::BlendFactor:   return D3D12_BLEND_BLEND_FACTOR;
//            case BlendFactor::InvBlendFactor:return D3D12_BLEND_INV_BLEND_FACTOR;
//            default:                         return D3D12_BLEND_ONE;
//        }
//    }
//
//    D3D12_BLEND_OP D3D12GraphicsPipeline::TranslateBlendOp(BlendOp op) const
//    {
//        switch (op)
//        {
//            case BlendOp::Add:         return D3D12_BLEND_OP_ADD;
//            case BlendOp::Subtract:    return D3D12_BLEND_OP_SUBTRACT;
//            case BlendOp::RevSubtract: return D3D12_BLEND_OP_REV_SUBTRACT;
//            case BlendOp::Min:         return D3D12_BLEND_OP_MIN;
//            case BlendOp::Max:         return D3D12_BLEND_OP_MAX;
//            default:                   return D3D12_BLEND_OP_ADD;
//        }
//    }
//
//    void D3D12GraphicsPipeline::ConfigureBlendStateForBlendMode(BlendMode mode, D3D12_RENDER_TARGET_BLEND_DESC& rtBlendDesc) const
//    {
//        switch (mode)
//        {
//            case BlendMode::Alpha:
//                rtBlendDesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
//                rtBlendDesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
//                rtBlendDesc.BlendOp = D3D12_BLEND_OP_ADD;
//                rtBlendDesc.SrcBlendAlpha = D3D12_BLEND_ONE;
//                rtBlendDesc.DestBlendAlpha = D3D12_BLEND_INV_SRC_ALPHA;
//                rtBlendDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
//                break;
//                
//            case BlendMode::Additive:
//                rtBlendDesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
//                rtBlendDesc.DestBlend = D3D12_BLEND_ONE;
//                rtBlendDesc.BlendOp = D3D12_BLEND_OP_ADD;
//                rtBlendDesc.SrcBlendAlpha = D3D12_BLEND_ONE;
//                rtBlendDesc.DestBlendAlpha = D3D12_BLEND_ONE;
//                rtBlendDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
//                break;
//                
//            case BlendMode::Multiply:
//                rtBlendDesc.SrcBlend = D3D12_BLEND_DEST_COLOR;
//                rtBlendDesc.DestBlend = D3D12_BLEND_ZERO;
//                rtBlendDesc.BlendOp = D3D12_BLEND_OP_ADD;
//                rtBlendDesc.SrcBlendAlpha = D3D12_BLEND_DEST_ALPHA;
//                rtBlendDesc.DestBlendAlpha = D3D12_BLEND_ZERO;
//                rtBlendDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
//                break;
//                
//            case BlendMode::None:
//            default:
//                rtBlendDesc.SrcBlend = D3D12_BLEND_ONE;
//                rtBlendDesc.DestBlend = D3D12_BLEND_ZERO;
//                rtBlendDesc.BlendOp = D3D12_BLEND_OP_ADD;
//                rtBlendDesc.SrcBlendAlpha = D3D12_BLEND_ONE;
//                rtBlendDesc.DestBlendAlpha = D3D12_BLEND_ZERO;
//                rtBlendDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
//                break;
//        }
//    }
//
//}
//

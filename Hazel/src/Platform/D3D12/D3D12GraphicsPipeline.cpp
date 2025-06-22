#include "hzpch.h"
#include "D3D12GraphicsPipeline.h"
#include "D3D12Shader.h"
#include "D3D12RenderAPIManager.h"

namespace Hazel {

    uint64_t D3D12GraphicsPipeline::s_NextHandleId = 1;

    D3D12GraphicsPipeline::D3D12GraphicsPipeline(const GraphicsPipelineDesc& desc)
        : m_Description(desc)
    {
        HZ_CORE_ASSERT(desc.shader, "Shader cannot be null");
        
        // 获取D3D12设备
        D3D12RenderAPIManager* renderAPIManager = dynamic_cast<D3D12RenderAPIManager*>(
            RenderAPIManager::getInstance()->GetManager().get());
        m_Device = renderAPIManager->GetD3DDevice();
        HZ_CORE_ASSERT(m_Device, "Failed to get D3D12 device");
        
        // 生成唯一句柄
        m_Handle.id = s_NextHandleId++;
        m_Handle.isValid = false;
        
        // 创建根签名和管线状态
        CreateRootSignature();
        CreatePipelineState();
        
        m_Handle.isValid = (m_PipelineState != nullptr);
    }

    D3D12GraphicsPipeline::~D3D12GraphicsPipeline()
    {
        // ComPtr会自动释放资源
    }

    void D3D12GraphicsPipeline::Bind() const
    {
        if (!IsValid()) {
            HZ_CORE_ERROR("Attempting to bind invalid pipeline state");
            return;
        }
        
        // 获取当前命令列表并设置管线状态
        D3D12RenderAPIManager* renderAPIManager = static_cast<D3D12RenderAPIManager*>(
            RenderAPIManager::getInstance()->GetManager().get());
        auto cmdList = renderAPIManager->GetCmdList();
        if (cmdList) {
            cmdList->SetPipelineState(m_PipelineState.Get());
            cmdList->SetGraphicsRootSignature(m_RootSignature.Get());
        }
    }

    PipelineStateHandle D3D12GraphicsPipeline::GetHandle() const
    {
        return m_Handle;
    }

    bool D3D12GraphicsPipeline::IsValid() const
    {
        return m_Handle.isValid && m_PipelineState && m_RootSignature;
    }

    void D3D12GraphicsPipeline::CreateRootSignature()
    {
        // 基于Shader反射创建根签名
        auto d3d12Shader = std::static_pointer_cast<D3D12Shader>(m_Description.shader);
        auto reflection = d3d12Shader->GetReflection();
        
        // 获取资源绑定信息
        auto resourceBindings = reflection->ReflectResourceBindings();
        
        std::vector<CD3DX12_ROOT_PARAMETER> rootParameters;
        std::vector<CD3DX12_DESCRIPTOR_RANGE> descriptorRanges;
        
        // 为每个资源绑定创建根参数
        for (const auto& binding : resourceBindings) {
            CD3DX12_DESCRIPTOR_RANGE range;
            
            switch (binding.Type) {
                case ResourceType::ConstantBuffer:
                    range.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, binding.BindPoint, binding.BindSpace);
                    break;
                case ResourceType::ShaderResource:
                    range.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, binding.BindPoint, binding.BindSpace);
                    break;
                case ResourceType::UnorderedAccess:
                    range.Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, binding.BindPoint, binding.BindSpace);
                    break;
                case ResourceType::Sampler:
                    range.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 1, binding.BindPoint, binding.BindSpace);
                    break;
            }
            
            descriptorRanges.push_back(range);
            
            CD3DX12_ROOT_PARAMETER rootParam;
            rootParam.InitAsDescriptorTable(1, &descriptorRanges.back());
            rootParameters.push_back(rootParam);
        }
        
        // 创建根签名描述
        CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
        rootSignatureDesc.Init(
            static_cast<UINT>(rootParameters.size()),
            rootParameters.data(),
            0, nullptr,
            D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT
        );
        
        // 序列化和创建根签名
        Microsoft::WRL::ComPtr<ID3DBlob> signature;
        Microsoft::WRL::ComPtr<ID3DBlob> error;
        HRESULT hr = D3D12SerializeRootSignature(
            &rootSignatureDesc,
            D3D_ROOT_SIGNATURE_VERSION_1_0,
            &signature,
            &error
        );
        
        if (FAILED(hr)) {
            HZ_CORE_ERROR("Failed to serialize root signature: {0}", 
                error ? (char*)error->GetBufferPointer() : "Unknown error");
            return;
        }
        
        hr = m_Device->CreateRootSignature(
            0,
            signature->GetBufferPointer(),
            signature->GetBufferSize(),
            IID_PPV_ARGS(&m_RootSignature)
        );
        
        HZ_CORE_ASSERT(SUCCEEDED(hr), "Failed to create root signature");
    }

    void D3D12GraphicsPipeline::CreatePipelineState()
    {
        auto d3d12Shader = std::static_pointer_cast<D3D12Shader>(m_Description.shader);
        
        // 创建管线状态描述
        D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
        
        // 着色器
        auto vsBlob = d3d12Shader->GetVSByteCode();
        auto psBlob = d3d12Shader->GetPSByteCode();
        
        if (vsBlob) {
            psoDesc.VS = { vsBlob->GetBufferPointer(), vsBlob->GetBufferSize() };
        }
        if (psBlob) {
            psoDesc.PS = { psBlob->GetBufferPointer(), psBlob->GetBufferSize() };
        }
        
        // 根签名
        psoDesc.pRootSignature = m_RootSignature.Get();
        
        // 渲染状态
        psoDesc.RasterizerState = ConvertRasterizerState(m_Description.rasterizerState);
        psoDesc.BlendState = ConvertBlendState(m_Description.blendState);
        psoDesc.DepthStencilState = ConvertDepthStencilState(m_Description.depthStencilState);
        
        // 输入布局
        const auto& d3d12InputLayout = d3d12Shader->GetD3D12InputLayout();
        psoDesc.InputLayout = { d3d12InputLayout.data(), static_cast<UINT>(d3d12InputLayout.size()) };
        
        // 图元拓扑
        psoDesc.PrimitiveTopologyType = ConvertPrimitiveTopology(m_Description.primitiveTopology);
        
        // 渲染目标格式
        psoDesc.NumRenderTargets = 1;
        psoDesc.RTVFormats[0] = ConvertTextureFormat(m_Description.colorFormat);
        psoDesc.DSVFormat = ConvertTextureFormat(m_Description.depthStencilFormat);
        
        // 采样描述
        psoDesc.SampleDesc.Count = m_Description.sampleCount;
        psoDesc.SampleDesc.Quality = m_Description.sampleQuality;
        psoDesc.SampleMask = UINT_MAX;
        
        // 创建管线状态对象
        HRESULT hr = m_Device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_PipelineState));
        HZ_CORE_ASSERT(SUCCEEDED(hr), "Failed to create graphics pipeline state");
    }

    D3D12_RASTERIZER_DESC D3D12GraphicsPipeline::ConvertRasterizerState(const RasterizerStateDesc& desc) const
    {
        D3D12_RASTERIZER_DESC rasterizerDesc = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
        
        rasterizerDesc.FillMode = ConvertFillMode(desc.fillMode);
        rasterizerDesc.CullMode = ConvertCullMode(desc.cullMode);
        rasterizerDesc.FrontCounterClockwise = desc.frontCounterClockwise;
        rasterizerDesc.DepthClipEnable = desc.depthClipEnable;
        // D3D12中scissor测试不是光栅化状态的一部分，需要通过RSSetScissorRects设置
        // rasterizerDesc.ScissorEnable = desc.scissorEnable; // D3D12中不支持
        rasterizerDesc.DepthBias = static_cast<INT>(desc.depthBias);
        rasterizerDesc.DepthBiasClamp = desc.depthBiasClamp;
        rasterizerDesc.SlopeScaledDepthBias = desc.slopeScaledDepthBias;
        
        return rasterizerDesc;
    }

    D3D12_BLEND_DESC D3D12GraphicsPipeline::ConvertBlendState(const BlendStateDesc& desc) const
    {
        D3D12_BLEND_DESC blendDesc = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
        
        blendDesc.AlphaToCoverageEnable = desc.alphaToCoverageEnable;
        blendDesc.IndependentBlendEnable = desc.independentBlendEnable;
        
        for (int i = 0; i < 8; ++i) {
            const auto& rtBlend = desc.renderTargetBlend[i];
            auto& d3d12RtBlend = blendDesc.RenderTarget[i];
            
            d3d12RtBlend.BlendEnable = rtBlend.blendEnable;
            d3d12RtBlend.SrcBlend = ConvertBlendFactor(rtBlend.srcColorBlendFactor);
            d3d12RtBlend.DestBlend = ConvertBlendFactor(rtBlend.dstColorBlendFactor);
            d3d12RtBlend.BlendOp = ConvertBlendOp(rtBlend.colorBlendOp);
            d3d12RtBlend.SrcBlendAlpha = ConvertBlendFactor(rtBlend.srcAlphaBlendFactor);
            d3d12RtBlend.DestBlendAlpha = ConvertBlendFactor(rtBlend.dstAlphaBlendFactor);
            d3d12RtBlend.BlendOpAlpha = ConvertBlendOp(rtBlend.alphaBlendOp);
            d3d12RtBlend.RenderTargetWriteMask = rtBlend.colorWriteMask;
        }
        
        return blendDesc;
    }

    D3D12_DEPTH_STENCIL_DESC D3D12GraphicsPipeline::ConvertDepthStencilState(const DepthStencilStateDesc& desc) const
    {
        D3D12_DEPTH_STENCIL_DESC depthStencilDesc = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
        
        depthStencilDesc.DepthEnable = desc.depthEnable;
        depthStencilDesc.DepthWriteMask = desc.depthWriteEnable ? D3D12_DEPTH_WRITE_MASK_ALL : D3D12_DEPTH_WRITE_MASK_ZERO;
        depthStencilDesc.DepthFunc = ConvertDepthFunc(desc.depthFunc);
        depthStencilDesc.StencilEnable = desc.stencilEnable;
        depthStencilDesc.StencilReadMask = desc.stencilReadMask;
        depthStencilDesc.StencilWriteMask = desc.stencilWriteMask;
        
        return depthStencilDesc;
    }

    D3D12_PRIMITIVE_TOPOLOGY_TYPE D3D12GraphicsPipeline::ConvertPrimitiveTopology(PrimitiveTopology topology) const
    {
        switch (topology) {
            case PrimitiveTopology::PointList: return D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
            case PrimitiveTopology::LineList:
            case PrimitiveTopology::LineStrip: return D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
            case PrimitiveTopology::TriangleList:
            case PrimitiveTopology::TriangleStrip: return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
            default:
                HZ_CORE_ASSERT(false, "Unknown primitive topology");
                return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        }
    }

    DXGI_FORMAT D3D12GraphicsPipeline::ConvertTextureFormat(GraphicsPipelineDesc::TextureFormat format) const
    {
        switch (format) {
            case GraphicsPipelineDesc::TextureFormat::RGBA8: return DXGI_FORMAT_R8G8B8A8_UNORM;
            case GraphicsPipelineDesc::TextureFormat::BGRA8: return DXGI_FORMAT_B8G8R8A8_UNORM;
            case GraphicsPipelineDesc::TextureFormat::RGBA16F: return DXGI_FORMAT_R16G16B16A16_FLOAT;
            case GraphicsPipelineDesc::TextureFormat::RGBA32F: return DXGI_FORMAT_R32G32B32A32_FLOAT;
            case GraphicsPipelineDesc::TextureFormat::DEPTH24STENCIL8: return DXGI_FORMAT_D24_UNORM_S8_UINT;
            case GraphicsPipelineDesc::TextureFormat::DEPTH32F: return DXGI_FORMAT_D32_FLOAT;
            default:
                HZ_CORE_ASSERT(false, "Unknown texture format");
                return DXGI_FORMAT_UNKNOWN;
        }
    }

    D3D12_CULL_MODE D3D12GraphicsPipeline::ConvertCullMode(CullMode mode) const
    {
        switch (mode) {
            case CullMode::None: return D3D12_CULL_MODE_NONE;
            case CullMode::Front: return D3D12_CULL_MODE_FRONT;
            case CullMode::Back: return D3D12_CULL_MODE_BACK;
            default:
                HZ_CORE_ASSERT(false, "Unknown cull mode");
                return D3D12_CULL_MODE_BACK;
        }
    }

    D3D12_FILL_MODE D3D12GraphicsPipeline::ConvertFillMode(FillMode mode) const
    {
        switch (mode) {
            case FillMode::Solid: return D3D12_FILL_MODE_SOLID;
            case FillMode::Wireframe: return D3D12_FILL_MODE_WIREFRAME;
            case FillMode::Point: return D3D12_FILL_MODE_SOLID; // D3D12 doesn't have point fill mode
            default:
                HZ_CORE_ASSERT(false, "Unknown fill mode");
                return D3D12_FILL_MODE_SOLID;
        }
    }

    D3D12_COMPARISON_FUNC D3D12GraphicsPipeline::ConvertDepthFunc(DepthFunc func) const
    {
        switch (func) {
            case DepthFunc::Never: return D3D12_COMPARISON_FUNC_NEVER;
            case DepthFunc::Less: return D3D12_COMPARISON_FUNC_LESS;
            case DepthFunc::LessEqual: return D3D12_COMPARISON_FUNC_LESS_EQUAL;
            case DepthFunc::Equal: return D3D12_COMPARISON_FUNC_EQUAL;
            case DepthFunc::NotEqual: return D3D12_COMPARISON_FUNC_NOT_EQUAL;
            case DepthFunc::GreaterEqual: return D3D12_COMPARISON_FUNC_GREATER_EQUAL;
            case DepthFunc::Greater: return D3D12_COMPARISON_FUNC_GREATER;
            case DepthFunc::Always: return D3D12_COMPARISON_FUNC_ALWAYS;
            default:
                HZ_CORE_ASSERT(false, "Unknown depth function");
                return D3D12_COMPARISON_FUNC_LESS;
        }
    }

    D3D12_BLEND D3D12GraphicsPipeline::ConvertBlendFactor(BlendFactor factor) const
    {
        switch (factor) {
            case BlendFactor::Zero: return D3D12_BLEND_ZERO;
            case BlendFactor::One: return D3D12_BLEND_ONE;
            case BlendFactor::SrcColor: return D3D12_BLEND_SRC_COLOR;
            case BlendFactor::InvSrcColor: return D3D12_BLEND_INV_SRC_COLOR;
            case BlendFactor::SrcAlpha: return D3D12_BLEND_SRC_ALPHA;
            case BlendFactor::InvSrcAlpha: return D3D12_BLEND_INV_SRC_ALPHA;
            case BlendFactor::DstColor: return D3D12_BLEND_DEST_COLOR;
            case BlendFactor::InvDstColor: return D3D12_BLEND_INV_DEST_COLOR;
            case BlendFactor::DstAlpha: return D3D12_BLEND_DEST_ALPHA;
            case BlendFactor::InvDstAlpha: return D3D12_BLEND_INV_DEST_ALPHA;
            case BlendFactor::ConstantColor: return D3D12_BLEND_BLEND_FACTOR;
            case BlendFactor::InvConstantColor: return D3D12_BLEND_INV_BLEND_FACTOR;
            default:
                HZ_CORE_ASSERT(false, "Unknown blend factor");
                return D3D12_BLEND_ONE;
        }
    }

    D3D12_BLEND_OP D3D12GraphicsPipeline::ConvertBlendOp(BlendOp op) const
    {
        switch (op) {
            case BlendOp::Add: return D3D12_BLEND_OP_ADD;
            case BlendOp::Subtract: return D3D12_BLEND_OP_SUBTRACT;
            case BlendOp::ReverseSubtract: return D3D12_BLEND_OP_REV_SUBTRACT;
            case BlendOp::Min: return D3D12_BLEND_OP_MIN;
            case BlendOp::Max: return D3D12_BLEND_OP_MAX;
            default:
                HZ_CORE_ASSERT(false, "Unknown blend operation");
                return D3D12_BLEND_OP_ADD;
        }
    }

} // namespace Hazel

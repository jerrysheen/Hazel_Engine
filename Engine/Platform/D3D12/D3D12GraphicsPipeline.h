#pragma once

#include "Runtime/Graphics/RHI/Interface/IGraphicsPipeline.h"
#include "Platform/D3D12/d3dx12.h"
#include "Platform/D3D12/d3dUtil.h"
#include <d3d12.h>
#include <wrl/client.h>

namespace Hazel {

    class D3D12GraphicsPipeline : public IGraphicsPipeline {
    public:
        D3D12GraphicsPipeline(const GraphicsPipelineDesc& desc);
        virtual ~D3D12GraphicsPipeline();
        
        // IGraphicsPipeline 实现
        virtual void Bind() const override;
        virtual const GraphicsPipelineDesc& GetDescription() const override { return m_Description; }
        virtual PipelineStateHandle GetHandle() const override;
        virtual void* GetNativeHandle() const override { return m_PipelineState.Get(); }
        virtual bool IsValid() const override;
        
        // D3D12特定接口
        ID3D12PipelineState* GetD3D12PipelineState() const { return m_PipelineState.Get(); }
        ID3D12RootSignature* GetD3D12RootSignature() const { return m_RootSignature.Get(); }
        
    private:
        void CreateRootSignature();
        void CreatePipelineState();
        
        // 状态转换函数
        D3D12_RASTERIZER_DESC ConvertRasterizerState(const RasterizerStateDesc& desc) const;
        D3D12_BLEND_DESC ConvertBlendState(const BlendStateDesc& desc) const;
        D3D12_DEPTH_STENCIL_DESC ConvertDepthStencilState(const DepthStencilStateDesc& desc) const;
        D3D12_PRIMITIVE_TOPOLOGY_TYPE ConvertPrimitiveTopology(PrimitiveTopology topology) const;
        DXGI_FORMAT ConvertTextureFormat(GraphicsPipelineDesc::TextureFormat format) const;
        
        // 枚举转换函数
        D3D12_CULL_MODE ConvertCullMode(CullMode mode) const;
        D3D12_FILL_MODE ConvertFillMode(FillMode mode) const;
        D3D12_COMPARISON_FUNC ConvertDepthFunc(DepthFunc func) const;
        D3D12_BLEND ConvertBlendFactor(BlendFactor factor) const;
        D3D12_BLEND_OP ConvertBlendOp(BlendOp op) const;
        
        GraphicsPipelineDesc m_Description;
        Microsoft::WRL::ComPtr<ID3D12PipelineState> m_PipelineState;
        Microsoft::WRL::ComPtr<ID3D12RootSignature> m_RootSignature;
        PipelineStateHandle m_Handle;
        
        // D3D12设备引用
        Microsoft::WRL::ComPtr<ID3D12Device> m_Device;
        
        // 用于生成唯一句柄
        static uint64_t s_NextHandleId;
    };

} // namespace Hazel

#pragma once

#include "Hazel/Renderer/GraphicsPipeline.h"
#include "D3D12RendererAPI.h"

#include <d3d12.h>
#include <wrl/client.h>

namespace Hazel {

    class D3D12GraphicsPipeline : public GraphicsPipeline {
    public:
        D3D12GraphicsPipeline(const GraphicsPipelineDesc& desc);
        virtual ~D3D12GraphicsPipeline();

        virtual void Bind() const override;

        // D3D12 特定接口
        ID3D12PipelineState* GetNativePipelineState() const { return m_PipelineState.Get(); }
        ID3D12RootSignature* GetRootSignature() const { return m_RootSignature.Get(); }

    private:
        void CreateRootSignature();
        void CreatePipelineState();

        // 将Hazel的枚举转换为D3D12枚举的工具函数
        D3D12_CULL_MODE TranslateCullMode(CullMode mode) const;
        D3D12_FILL_MODE TranslateFillMode(FillMode mode) const;
        D3D12_PRIMITIVE_TOPOLOGY_TYPE TranslatePrimitiveTopology(PrimitiveTopology topology) const;
        D3D12_COMPARISON_FUNC TranslateDepthFunc(DepthFunc func) const;
        D3D12_BLEND TranslateBlendFactor(BlendFactor factor) const;
        D3D12_BLEND_OP TranslateBlendOp(BlendOp op) const;
        void ConfigureBlendStateForBlendMode(BlendMode mode, D3D12_RENDER_TARGET_BLEND_DESC& rtBlendDesc) const;

        Microsoft::WRL::ComPtr<ID3D12PipelineState> m_PipelineState;
        Microsoft::WRL::ComPtr<ID3D12RootSignature> m_RootSignature;
        
        // 缓存原始描述
        GraphicsPipelineDesc m_Description;

        // D3D12设备引用，避免重复获取
        ID3D12Device* m_Device;
    };

}


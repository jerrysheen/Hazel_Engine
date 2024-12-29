#pragma once

#include "Hazel.h"
#include "imgui/imgui.h"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "Panels/SceneHierarchyPanel.h"
#include "Platform/D3D12/d3dx12.h"
#include "Platform/D3D12/d3dUtil.h"
#include "Platform/D3D12/D3D12RenderAPIManager.h"

// temp:
#include "platform/D3D12/d3dUtil.h"
namespace Hazel
{
	class SceneViewLayer : public Layer
	{
	public:
		SceneViewLayer(Window& window);
		virtual ~SceneViewLayer() = default;
		virtual void OnAttach() override;
		virtual void OnDetach() override;
		void OnUpdate(Timestep ts) override;
		virtual void OnImGuiRender() override;
		void OnEvent(Event& e) override;

	private:
		Window& m_window;
		Ref<Shader> m_PbrShader;
		Ref<TextureBuffer> m_BackBuffer;
		Microsoft::WRL::ComPtr<ID3D12Resource> colorBuffer;
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle;
		CD3DX12_CPU_DESCRIPTOR_HANDLE srvHandle;
		D3D12_CPU_DESCRIPTOR_HANDLE my_texture_srv_cpu_handle;
		D3D12_GPU_DESCRIPTOR_HANDLE my_texture_srv_gpu_handle;

		std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayout;
		ComPtr<ID3D12PipelineState> mPSO = nullptr;
		XMFLOAT4X4 mWorld = MathHelper::Identity4x4();
		XMFLOAT4X4 mView = MathHelper::Identity4x4();
		XMFLOAT4X4 mProj = MathHelper::Identity4x4();
		std::unique_ptr<MeshGeometry> mBoxGeo = nullptr;
		ComPtr<ID3D12RootSignature> mRootSignature = nullptr;

		ComPtr<ID3DBlob> mvsByteCode = nullptr;
		ComPtr<ID3DBlob> mpsByteCode = nullptr;

	};
}


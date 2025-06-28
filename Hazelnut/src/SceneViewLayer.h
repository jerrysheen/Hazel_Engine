#pragma once

#include "Hazel.h"
#include "imgui.h"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "Panels/SceneHierarchyPanel.h"
#include "Platform/D3D12/d3dx12.h"
#include "Platform/D3D12/d3dUtil.h"
#include "Platform/D3D12/D3D12RenderAPIManager.h"
#include "Hazel/Material/Material.h"
// temp:
#include "platform/D3D12/d3dUtil.h"
namespace Hazel
{

	struct ObjectConstants
	{
		XMFLOAT4X4 WorldViewProj = MathHelper::Identity4x4();
	};

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
		inline uint64_t getCurrentFrameId() { return currentFrameID; };
		Window& m_window;
		Ref<Material> material;
		Ref<Shader> m_ColorShader;
		Ref<TextureBuffer> m_BackBuffer;
		Ref<TextureBuffer> m_DepthBuffer;
		Microsoft::WRL::ComPtr<ID3D12Resource> colorBuffer;
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle;
		CD3DX12_CPU_DESCRIPTOR_HANDLE srvHandle;
		D3D12_CPU_DESCRIPTOR_HANDLE my_texture_srv_cpu_handle;
		D3D12_GPU_DESCRIPTOR_HANDLE my_texture_srv_gpu_handle;
		bool m_TextureRegisteredToImGui = false;

		std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayout;
		ComPtr<ID3D12PipelineState> mPSO = nullptr;
		XMFLOAT4X4 mWorld = MathHelper::Identity4x4();
		XMFLOAT4X4 mView = MathHelper::Identity4x4();
		XMFLOAT4X4 mProj = MathHelper::Identity4x4();
		std::unique_ptr<MeshGeometry> mBoxGeo = nullptr;
		ComPtr<ID3D12RootSignature> mRootSignature = nullptr;

		ComPtr<ID3DBlob> mvsByteCode = nullptr;
		ComPtr<ID3DBlob> mpsByteCode = nullptr;

		Microsoft::WRL::ComPtr<ID3D12Fence> mFence;
		UINT64 mCurrentFence = 0;
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> mCommandQueue;
		D3D12_VIEWPORT mScreenViewport;
		D3D12_RECT mScissorRect;
		uint64_t currentFrameID = 0;
		//std::unique_ptr<UploadBuffer<ObjectConstants>> mObjectCB = nullptr;

		Ref<ConstantBuffer> objectCB;
		Ref<Mesh> mesh;
	};


}


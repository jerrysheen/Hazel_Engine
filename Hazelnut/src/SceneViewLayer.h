#pragma once

#include "Hazel.h"
#include "imgui/imgui.h"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "Panels/SceneHierarchyPanel.h"
#include "Platform/D3D12/d3dx12.h"
#include "Platform/D3D12/d3dUtil.h"
#include "Platform/D3D12/D3D12RenderAPIManager.h"


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
		Ref<Framebuffer> m_BackBuffer;
		Microsoft::WRL::ComPtr<ID3D12Resource> colorBuffer;
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle;
		CD3DX12_CPU_DESCRIPTOR_HANDLE srvHandle;
		D3D12_CPU_DESCRIPTOR_HANDLE my_texture_srv_cpu_handle;
		D3D12_GPU_DESCRIPTOR_HANDLE my_texture_srv_gpu_handle;
	};
}


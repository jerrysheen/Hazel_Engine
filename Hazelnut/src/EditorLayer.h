#pragma once

#include "Hazel.h"
#include "imgui/imgui.h"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"


namespace Hazel 
{
	class EditorLayer : public Layer
	{
	public:
		EditorLayer(Window& window);
		virtual ~EditorLayer() = default;
		virtual void OnAttach() override;
		virtual void OnDetach() override;
		void OnUpdate(Timestep ts) override;
		virtual void OnImGuiRender() override;
		void OnEvent(Event& e) override;

	private:
		Model* model;
		glm::vec2 playerPosition = { 0.0f, 0.0f };
		float playerMoveSpeed = 1;
		Window& m_window;
		Ref<Framebuffer> m_FrameBuffer;
		Ref<Framebuffer> m_ShadowMapRenderTarget;
		Ref<Shader> m_MainLightShader;
		Ref<Shader> m_PBRshader;
		Ref<Shader> m_UnLit;
		PerspectiveCameraController m_CameraController;

		//OrthographicCameraController m_ShadowCameraController;
		glm::vec2 m_viewPortPanelSize;
		bool m_ViewPortFocused;

		float m_LightPos[3] = { 13.75, 12, 10 };

		FramebufferSpecification m_fbSpec;
		FramebufferSpecification m_shadowMapSpec;

		Ref<Scene> m_ActiveScene;
		Entity m_GunObj;
		Entity m_Plane;
		Entity m_SkyBox;
		
	};
}


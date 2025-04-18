#pragma once

#include "Hazel.h"
#include "imgui/imgui.h"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "Panels/SceneHierarchyPanel.h"

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
//		Model* model;
		glm::vec2 playerPosition = { 0.0f, 0.0f };
		float playerMoveSpeed = 1;
		Window& m_window;
		//我只需要一个FrameBuffer，绑定不同的Texture即可。
		Ref<TextureBuffer> m_FrameBuffer;
		Ref<Texture2D> m_OpaqueTexture;
		Ref<Texture2D> m_DepthTexture;
		Ref<Texture2D> m_ShadowMap;
		//Ref<Framebuffer> m_ShadowMapRenderTarget;
		Ref<Shader> m_MainLightShader;
		Ref<Shader> m_PBRTextureShader;
		Ref<Shader> m_UnLitShader;
		Ref<Shader> m_SkyboxShader;
		Ref<Shader> m_PBRShader;
		// temp
		Ref<Shader> equirectangularToCubemapShader;
		PerspectiveCameraController m_CameraController;

		//OrthographicCameraController m_ShadowCameraController;
		glm::vec2 m_viewPortPanelSize;
		bool m_ViewPortFocused;
		RenderTargetEnum m_renderTargetEnum = RenderTargetEnum::OPAQUE_TEXTURE;
		float m_LightPos[3] = { 13.75, 12, 10 };
		float m_DiffuseColor[4] = { 1.0, 1.0, 1.0, 1.0 };
		float m_Metallic = 0.4;
		float m_Roughness = 0.4;

		TextureBufferSpecification m_fbSpec;
		TextureBufferSpecification m_shadowMapSpec;
		Ref<Texture3D> m_IrradianceMap;
		Ref<Scene> m_ActiveScene;
		Entity m_GunObj;
		Entity m_Plane;
		Entity m_Sphere;
		Entity m_SkyBox;
		Entity m_Cube;
		
		SceneHierarchyPanel m_SceneHierarchyPanel;
	};
}


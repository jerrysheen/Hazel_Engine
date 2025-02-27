#pragma once
#pragma once

#include "Hazel.h"
#include "imgui/imgui.h"

class SceneLayer : public Hazel::Layer
{
public:
	SceneLayer(Hazel::Window& window);
	virtual ~SceneLayer() = default;
	virtual void OnAttach() override;
	virtual void OnDetach() override;
	void OnUpdate(Hazel::Timestep ts) override;
	virtual void OnImGuiRender() override;
	void OnEvent(Hazel::Event& e) override;

private:
	//glm::vec2 playerPosition = { 0.0f, 0.0f };
	//float playerMoveSpeed = 1;
	//float particleXdir = 0.0f, particleYdir = 0.0f;
	Hazel::Window& m_window;

	//Hazel::Ref<Hazel::Shader> m_FlatColorShader;
	//Hazel::Ref<Hazel::VertexArray> m_SquareVA;
	//Hazel::Ref<Hazel::Texture2D> m_Texture;
	Hazel::Ref<Hazel::TextureBuffer> m_FrameBuffer;

	Hazel::PerspectiveCameraController m_CameraController;
	//glm::vec4 m_SquareColor = { 0.2f, 0.3f, 0.8f, 1.0f };
};
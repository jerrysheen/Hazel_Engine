#include "ThreeDLayer.h"
#include "Platform/OpenGL/OpenGLShader.h"



ThreeDLayer::ThreeDLayer(Hazel::Window& window)
	:Layer("3Dlayer"),
	m_CameraController(45.0f, window.GetWidth(), window.GetHeight(), 0.1f, 100.0f),
	m_window(window)
{
	HZ_INFO("{0}, {1}", window.GetWidth(), window.GetHeight());
}

void ThreeDLayer::OnAttach()
{
	m_Texture = Hazel::Texture2D::Create("assets/textures/Checkerboard.png");

}

void ThreeDLayer::OnDetach()
{

}

void ThreeDLayer::OnUpdate(Hazel::Timestep ts)
{

	Hazel::RendererCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
	Hazel::RendererCommand::Clear();



	Hazel::Renderer2D::BeginScene(m_CameraController.GetCamera());
	Hazel::Renderer2D::DrawQuad({ 0.0f, 0.0f }, { 0.8f, 0.8f }, { 0.8f, 0.2f, 0.3f, 1.0f });
	Hazel::Renderer2D::DrawQuad({ 0.5f, -0.5f }, { 0.5f, 0.75f }, { 0.2f, 0.3f, 0.8f, 1.0f });
	Hazel::Renderer2D::DrawQuad({ 0.0f, 0.0f, -0.1f }, { 10.0f, 10.0f }, m_Texture);
	Hazel::Renderer2D::EndScene();

	m_CameraController.OnUpdate(ts);
}

void ThreeDLayer::OnImGuiRender()
{
	ImGui::Begin("Settings");
	//ImGui::ColorEdit3("Square Color", glm::value_ptr(m_SquareColor));

	ImGui::End();
}

void ThreeDLayer::OnEvent(Hazel::Event& e)
{
	m_CameraController.OnEvent(e);
}

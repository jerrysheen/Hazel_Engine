#include "SceneLayer.h"
#include "Platform/OpenGL/OpenGLShader.h"



SceneLayer::SceneLayer(Hazel::Window& window)
	:Layer("3Dlayer"),
	m_CameraController(45.0f, window.GetWidth(), window.GetHeight(), 0.1f, 100000.0f),
	m_window(window)
{
	HZ_INFO("{0}, {1}", window.GetWidth(), window.GetHeight());
}

void SceneLayer::OnAttach()
{
	m_Texture = Hazel::Texture2D::Create("assets/textures/Checkerboard.png");

}

void SceneLayer::OnDetach()
{

}

void SceneLayer::OnUpdate(Hazel::Timestep ts)
{

	Hazel::RendererCommand::SetClearColor({ 104.0/256.0, 97.0 / 256.0, 92.0 / 256.0, 1 });
	Hazel::RendererCommand::Clear();

	Hazel::Renderer3D::BeginScene(m_CameraController.GetCamera());
	Hazel::Renderer3D::DrawPrimitives();
	
	Hazel::Renderer3D::EndScene();

	m_CameraController.OnUpdate(ts);
}

void SceneLayer::OnImGuiRender()
{
	ImGui::Begin("Settings");
	//ImGui::ColorEdit3("Square Color", glm::value_ptr(m_SquareColor));
	if (ImGui::Button("Add a new Plane")) 
	{
		Hazel::Renderer3D::CreatePlane();
	}
	ImGui::End();
}

void SceneLayer::OnEvent(Hazel::Event& e)
{
	m_CameraController.OnEvent(e);
}

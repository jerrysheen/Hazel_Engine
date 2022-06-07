#include "EditorLayer.h"
#include "Platform/OpenGL/OpenGLShader.h"


namespace Hazel
{
	EditorLayer::EditorLayer(Window& window)
		:Layer("EditorLayer"),
		m_window(window)
	{

	}

	void EditorLayer::OnAttach()
	{
		//m_Texture = Texture2D::Create("assets/textures/Checkerboard.png");
	}

	void EditorLayer::OnDetach()
	{

	}

	void EditorLayer::OnUpdate(Timestep ts)
	{
		//HZ_PROFILE_FUNCTION();
		//{
		//	HZ_PROFILE_SCOPE("CameraController::OnUpdate");
		//	m_CameraController.OnUpdate(ts);
		//}


		//{
		//	HZ_PROFILE_SCOPE("Renderer Prep");
		//	RendererCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		//	RendererCommand::Clear();
		//}

		//{
		//	HZ_PROFILE_SCOPE("Renderer Draw");
		//	Renderer3D::BeginScene(m_CameraController.GetCamera());
		//	Renderer3D::DrawQuad({ -1.0f, 0.0f }, { 0.8f, 0.8f }, { 0.8f, 0.2f, 0.3f, 1.0f });
		//	Renderer3D::DrawQuad({ 0.5f, -0.5f }, { 0.5f, 0.75f }, { 0.2f, 0.3f, 0.8f, 1.0f });
		//	Renderer3D::DrawQuad({ 0.0f, 0.0f, -0.1f }, { 10.0f, 10.0f }, m_Texture);
		//	Renderer3D::EndScene();
		//}
	}

	void EditorLayer::OnImGuiRender()
	{
		ImGui::Begin("Settings");

		ImGui::End();
	}

	void EditorLayer::OnEvent(Event& e)
	{
	}

}
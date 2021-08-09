#include "Sandbox2D.h"
#include "Platform/OpenGL/OpenGLShader.h"
#include "ParticleSystem.h"



Sandbox2D::Sandbox2D(Hazel::Window& window)
	:Layer("Sandbox2D"),
	m_CameraController(1280.0f / 720.0f),
	m_window(window)
{

}

void Sandbox2D::OnAttach()
{
	m_Texture = Hazel::Texture2D::Create("assets/textures/Checkerboard.png");
	m_ParticleProps.ColorBegin = { 0.99f, 0.99f, 0.99f, 1.0f };
	m_ParticleProps.ColorEnd = { 0.99f, 0.5f, 0.2f, 1.0f };
	m_ParticleProps.SizeBegin = 0.05f; m_ParticleProps.SizeVariation = 0.02f; m_ParticleProps.SizeEnd = 0.01f;
	m_ParticleProps.LifeTime = 1.0f;
	m_ParticleProps.Velocity = { 0.0, 0.0 };
	m_ParticleProps.VelocityVariation = { 1.0f, 1.0f };
	m_ParticleProps.Position = { 0.0f, 0.0f };

}

void Sandbox2D::OnDetach()
{

}

void Sandbox2D::OnUpdate(Hazel::Timestep ts)
{
	HZ_PROFILE_FUNCTION();
	{
		HZ_PROFILE_SCOPE("CameraController::OnUpdate");
		m_CameraController.OnUpdate(ts);
	}


	{
		HZ_PROFILE_SCOPE("Renderer Prep");
		Hazel::RendererCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		Hazel::RendererCommand::Clear();
	}

	{
		HZ_PROFILE_SCOPE("Renderer Draw");
		Hazel::Renderer2D::BeginScene(m_CameraController.GetCamera());
		Hazel::Renderer2D::DrawQuad({ -1.0f, 0.0f }, { 0.8f, 0.8f }, { 0.8f, 0.2f, 0.3f, 1.0f });
		Hazel::Renderer2D::DrawQuad({ 0.5f, -0.5f }, { 0.5f, 0.75f }, { 0.2f, 0.3f, 0.8f, 1.0f });
		Hazel::Renderer2D::DrawQuad({ 0.0f, 0.0f, -0.1f }, { 10.0f, 10.0f }, *m_Texture);
		Hazel::Renderer2D::EndScene();
	}
}

void Sandbox2D::OnImGuiRender()
{
	ImGui::Begin("Settings");
	ImGui::ColorEdit3("Square Color", glm::value_ptr(m_SquareColor));

	ImGui::End();
}

void Sandbox2D::OnEvent(Hazel::Event& e)
{
	m_CameraController.OnEvent(e);
}

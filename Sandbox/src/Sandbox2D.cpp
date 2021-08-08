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
	m_CameraController.OnUpdate(ts);


	Hazel::RendererCommand::SetClearColor({ 0.1, 0.1, 0.1, 1 });
	Hazel::RendererCommand::Clear();


	Hazel::Renderer2D::BeginScene(m_CameraController.GetCamera());

	if (Hazel::Input::IsKeyPressed(HZ_KEY_UP)) {
		playerPosition.y += playerMoveSpeed * ts;
		particleYdir -= 0.1 * ts;
	}
	else if (Hazel::Input::IsKeyPressed(HZ_KEY_DOWN)) {
		playerPosition.y -= playerMoveSpeed * ts;
		particleYdir += 0.1 * ts;
	}
	if (Hazel::Input::IsKeyPressed(HZ_KEY_RIGHT)) {
		playerPosition.x += playerMoveSpeed * ts;
		particleXdir -= 0.1 * ts;
	}
	else if (Hazel::Input::IsKeyPressed(HZ_KEY_LEFT)) {
		playerPosition.x -= playerMoveSpeed * ts;
		particleXdir += 0.1 * ts;
	}
	HZ_INFO("{0},  {1}", particleXdir, particleYdir);

	Hazel::Renderer2D::DrawQuad({ -0.3f, -0.7f, 0.6f }, { 0.5f, 1.0f }, { 0.8f, 0.2f,0.3f,1.0f });
	Hazel::Renderer2D::DrawQuad(glm::vec3( playerPosition, 0.1f ), { 0.3f, 0.3f }, *m_Texture);
	
		
	//	m_ParticleProps.Position = playerPosition;
	//	m_ParticleProps.Velocity = glm::vec2(particleXdir, particleYdir);
	//	for (int i = 0; i < 1; i++) {
	//		particleSystem.Emit(m_ParticleProps);
	//	}

	//

	//particleSystem.Update(ts);
	//particleSystem.Render();

	Hazel::Renderer2D::EndScene();


	//std::dynamic_pointer_cast<Hazel::OpenGLShader>(m_FlatColorShader)->Bind();
	//std::dynamic_pointer_cast<Hazel::OpenGLShader>(m_FlatColorShader)->UploadUniformFloat4("u_Color", m_SquareColor);

	//Hazel::Renderer::Submit(m_FlatColorShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));

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

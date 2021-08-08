#include "Hazel.h"
#include "Hazel/Core/EntryPoint.h"
#include "imgui/imgui.h"
#include "Hazel/Events/Event.h"

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "Platform/OpenGL/OpenGLShader.h"
#include "Sandbox2D.h"

class ExampleLayer : public Hazel::Layer
{
public:
	ExampleLayer()
		: Layer("Example"),
		m_CameraController(1280.0f / 720.0f)
	{
		m_VertexArray = Hazel::VertexArray::Create();
		m_VertexArray->Bind();


		float vertices[3 * 7] = {
			-0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f,
			 0.5f, -0.5f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f,
			 0.0f,  0.5f, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f
		};


		Hazel::Ref<Hazel::VertexBuffer> m_VertexBuffer;
		m_VertexBuffer.reset(Hazel::VertexBuffer::Create(vertices, sizeof(vertices)));
		{
			Hazel::BufferLayout layout = {
				{ Hazel::ShaderDataType::Float3, "a_Position"},
				{ Hazel::ShaderDataType::Float4, "a_Color" }
			};
			m_VertexBuffer->SetLayout(layout);
		}
		m_VertexArray->AddVertexBuffer(m_VertexBuffer);
		uint32_t indices[3] = { 0, 1, 2 };
		Hazel::Ref<Hazel::IndexBuffer> m_IndexBuffer;
		m_IndexBuffer.reset(Hazel::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));
		m_VertexArray->SetIndexBuffer(m_IndexBuffer);

		m_SquareVA = Hazel::VertexArray::Create();
		float squareVertices[5 * 4] = {
			-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
			 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
			 0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
			-0.5f,  0.5f, 0.0f, 0.0f, 1.0f
		};
		//	float squareVertices[5 * 4] = {
		//		 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		//		 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		//		 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
		//		0.0f,  1.0f, 0.0f, 0.0f, 1.0f
		//};
		Hazel::Ref<Hazel::VertexBuffer> squareVB;
		squareVB.reset(Hazel::VertexBuffer::Create(squareVertices, sizeof(squareVertices)));
		squareVB->SetLayout({
			{ Hazel::ShaderDataType::Float3, "a_Position" },
			{ Hazel::ShaderDataType::Float2, "a_Texcoord"}

			});
		m_SquareVA->AddVertexBuffer(squareVB);

		uint32_t squareIndices[6] = { 0, 1, 2, 2, 3, 0 };
		Hazel::Ref<Hazel::IndexBuffer> squareIB;
		squareIB.reset(Hazel::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t)));
		m_SquareVA->SetIndexBuffer(squareIB);

		std::string vertexSrc = R"(
			#version 330 core

			layout(location = 0) in vec4 a_Position;
			layout(location = 1) in vec4 a_Color;
			
			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;

			out vec4 v_Color;
			out vec4 v_Position;

			void main()
			{
				v_Position = a_Position;
				v_Color = a_Color;
				gl_Position = u_ViewProjection * u_Transform * a_Position;
			}	

		)";

		std::string fragmentSrc = R"(
			#version 330 core
			
			in vec4 v_Color;
			layout(location = 0) out vec4 color;
			void main()
			{
				color = v_Color;
			}	

		)";
		m_Shader = Hazel::Shader::Create("VertexPosColor",vertexSrc, fragmentSrc);

		std::string flatShaderVertexSrc = R"(
			#version 330 core
			
			layout(location = 0) in vec3 a_Position;
			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;

			out vec3 v_Position;
			void main()
			{
				v_Position = a_Position;
				gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);	
			}
		)";

		std::string flatShaderFragmentSrc = R"(
			#version 330 core
			
			layout(location = 0) out vec4 color;
			uniform vec3 u_Color;

			in vec3 v_Position;
			void main()
			{
				color = vec4(u_Color, 1.0f);
			}
		)";

		m_FlatColorShader = Hazel::Shader::Create("FlatColor", flatShaderVertexSrc, flatShaderFragmentSrc);

		auto textureShader = m_ShaderLibrary.Load("assets/shaders/Texture.glsl");
		//m_TextureShader = Hazel::Shader::Create("assets/shaders/Texture.glsl");

		std::dynamic_pointer_cast<Hazel::OpenGLShader>(textureShader)->Bind();
		std::dynamic_pointer_cast<Hazel::OpenGLShader>(textureShader)->UploadUniformInt("u_Texture", 0);

		m_Texture = Hazel::Texture2D::Create("assets/textures/Checkerboard.png");
		m_ChernoLogo = Hazel::Texture2D::Create("assets/textures/ChernoLogo.png");
	}

	void OnUpdate(Hazel::Timestep ts) override
	{	
		//HZ_TRACE("Delta time: {0}s, {1}ms", ts.GetSeconds(), ts.GetMilliseconds());
		//HZ_TRACE("Color Value {0}, {1}, {2}", m_SquareColor.r, m_SquareColor.g, m_SquareColor.b);
		// get input 
		// 
		
		m_CameraController.OnUpdate(ts);


		Hazel::RendererCommand::SetClearColor({ 0.1, 0.1, 0.1, 1 });
		Hazel::RendererCommand::Clear();

		
		Hazel::Renderer::BeginScene(m_CameraController.GetCamera());

		glm::mat4 scale = glm::scale(glm::mat4(1.0), glm::vec3(0.1f));
		
		glm::vec4 redColor(0.8f, 0.2f, 0.3f, 1.0f);
		glm::vec4 blueColor(0.2f, 0.3f, 0.8f, 1.0f);

		std::dynamic_pointer_cast<Hazel::OpenGLShader>(m_FlatColorShader)->Bind();
		std::dynamic_pointer_cast<Hazel::OpenGLShader>(m_FlatColorShader)->UploadUniformFloat3("u_Color", m_SquareColor);

		for (int y = 0; y < 20; y++) {
			for (int x = 0; x < 20; x++)
			{
				glm::vec3 pos(x * 0.11f, y * 0.11f, 0.0f);
				glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) * scale;
				Hazel::Renderer::Submit(m_FlatColorShader, m_SquareVA, transform);

			}
		}
		auto textureShader =  m_ShaderLibrary.Get("Texture");

		m_Texture->Bind(0);
		Hazel::Renderer::Submit(textureShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));
		m_ChernoLogo->Bind(0);
		Hazel::Renderer::Submit(textureShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));
		//Hazel::Renderer::Submit(m_Shader, m_VertexArray);

		Hazel::Renderer::EndScene();
	}


	virtual void OnImGuiRender() override
	{
		ImGui::Begin("Settings");
		ImGui::ColorEdit3("Square Color", glm::value_ptr(m_SquareColor));
		ImGui::End();
	}

	void OnEvent(Hazel::Event& e) override
	{
		m_CameraController.OnEvent(e);
	}

private:
	Hazel::ShaderLibrary m_ShaderLibrary;

	Hazel::Ref<Hazel::Shader> m_Shader;
	Hazel::Ref<Hazel::VertexArray> m_VertexArray;

	Hazel::Ref<Hazel::Shader> m_FlatColorShader;
	Hazel::Ref<Hazel::VertexArray> m_SquareVA;

	//Hazel::Ref<Hazel::Shader> m_TextureShader;

	Hazel::Ref<Hazel::Texture2D> m_Texture, m_ChernoLogo;

	Hazel::OrthographicCameraController m_CameraController;
	glm::vec3 m_SquareColor = {0.2f, 0.3f, 0.8f};
	
};

class Sandbox : public Hazel::Application
{
public:
	Sandbox() 
	{
		// ExapleLayer helps us to see the effect imediately.
		// PushLayer(new ExampleLayer());
		PushLayer(new Sandbox2D(GetWindow()));
		
	}
	~Sandbox() {
	}
};

Hazel::Application* Hazel::CreateApplication() 
{
	return new Sandbox();
}
#include "hzpch.h"
#include "Renderer3D.h"


#include "RendererCommand.h"
#include "Renderer.h"

namespace Hazel {

	std::vector<Renderer3D::Renderer3DStorage*>* Renderer3D::s_ObjData = new std::vector<Renderer3D::Renderer3DStorage*>();
	glm::mat4  Renderer3D::m_ViewProjection;
	glm::vec3  Renderer3D::m_CameraPos;

	static Renderer3D::Renderer3DStorage* s_Data;

	void Renderer3D::Init()
	{
		
		s_ObjData = new std::vector<Renderer3D::Renderer3DStorage*>();
		s_Data = new Renderer3DStorage();
		RenderGround();
	}
	void Renderer3D::Shutdown()
	{
		delete s_ObjData;
		delete s_Data;
	}
	void Renderer3D::BeginScene(const OrthographicCamera& camera)
	{
		m_ViewProjection = camera.GetViewProjectionMatrix();
		
	}

	void Renderer3D::BeginScene(const PerspectiveCamera& camera)
	{

		m_ViewProjection = camera.GetViewProjectionMatrix();
		m_CameraPos = camera.GetCamPos();
		//HZ_CORE_INFO("x: {0}, y :  {1} " , m_ViewProjection[0][0], m_ViewProjection[1][1]);

		
	}

	void Renderer3D::EndScene()
	{
	}

	void Renderer3D::CreatePlane()
	{
		CreatePlane({0.0f, 0.0f, 0.0f});
	}

	void Renderer3D::RenderGround() 
	{
		Renderer3D::Renderer3DStorage* s_Data = new Renderer3DStorage();
		s_Data->QuadVertexArray = VertexArray::Create();
		Ref<VertexBuffer> squareVB;
		squareVB.reset(VertexBuffer::Create(Renderer3D::m_ground, sizeof(m_ground)));
		squareVB->SetLayout({
				{ ShaderDataType::Float3, "a_Position" },
				//{ ShaderDataType::	Float2, "a_TexCoord" }

			});
		s_Data->QuadVertexArray->AddVertexBuffer(squareVB);

		Ref<IndexBuffer> squareIB;
		squareIB.reset(IndexBuffer::Create(m_groundIndices, sizeof(m_groundIndices) / sizeof(uint32_t)));
		s_Data->QuadVertexArray->SetIndexBuffer(squareIB);

		s_Data->WhiteTexture = Texture2D::Create(1, 1);
		uint32_t whiteTextureData = 0xffffffff;
		s_Data->WhiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));

		//HZ_CORE_INFO("x : {0}", s_ObjData->size());
		s_Data->TextureShader = Shader::Create("assets/shaders/Ground.glsl");

		s_Data->Color = std::make_shared<glm::vec4>(1.0, 1.0, 1.0, 1.0);
		s_Data->Scale = std::make_shared<glm::vec3>(1.0, 1.0, 1.0);
		s_Data->DrawType = DRAW_TYPE::HZ_LINES;
		s_ObjData->push_back(s_Data);
	}


	void Renderer3D::CreatePlane(const glm::vec3& position)
	{
		Renderer3D::Renderer3DStorage*  s_Data = new Renderer3DStorage();
		
		s_Data->QuadVertexArray = VertexArray::Create();


		Ref<VertexBuffer> squareVB;
		squareVB.reset(VertexBuffer::Create(Renderer3D::m_squareVertices, sizeof(m_squareVertices)));
		squareVB->SetLayout({
				{ ShaderDataType::Float3, "a_Position" },
				{ ShaderDataType::Float2, "a_TexCoord" }

			});
		s_Data->QuadVertexArray->AddVertexBuffer(squareVB);

		uint32_t squareIndices[6] = { 0, 1, 2, 2, 3, 0 };
		Ref<IndexBuffer> squareIB;
		squareIB.reset(IndexBuffer::Create(m_squareIndices, sizeof(m_squareIndices) / sizeof(uint32_t)));
		s_Data->QuadVertexArray->SetIndexBuffer(squareIB);

		s_Data->WhiteTexture = Texture2D::Create(1, 1);
		uint32_t whiteTextureData = 0xffffffff;
		s_Data->WhiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));
	
		//HZ_CORE_INFO("x : {0}", s_ObjData->size());
		s_Data->TextureShader = Shader::Create("assets/shaders/Texture.glsl");

		s_Data->Color = std::make_shared<glm::vec4>(1.0, 1.0, 1.0, 1.0);
		s_Data->Scale = std::make_shared<glm::vec3>(1.0, 1.0, 1.0);
		s_Data->DrawType = DRAW_TYPE::HZ_TRIANGLES;
		s_ObjData->push_back(s_Data);
	}


	void Renderer3D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color)
	{
		DrawQuad({ position.x, position.y, 0.0f }, size, color);
	}
	void Renderer3D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
	{

		s_Data->TextureShader->Bind();
		s_Data->TextureShader->SetFloat4("u_Color", color);
		s_Data->TextureShader->SetFloat("u_TilingFactor", 1.0f);
		s_Data->WhiteTexture->Bind(0);
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) 
			* glm::scale(glm::mat4(1.0f), glm::vec3(size.x, size.y, 1.0f));
		s_Data->TextureShader->SetMat4("u_Transform", transform);

		s_Data->QuadVertexArray->Bind();
		//RendererCommand::DrawIndexed(s_Data->QuadVertexArray);
	}

	void Renderer3D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		DrawQuad({ position.x, position.y, 0.0f }, size, texture, tilingFactor, tintColor);
	}
	void Renderer3D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		s_Data->TextureShader->SetFloat4("u_Color", tintColor);
		s_Data->TextureShader->SetFloat("u_TilingFactor", tilingFactor);
		texture->Bind(0);

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });
		s_Data->TextureShader->SetMat4("u_Transform", transform);

		s_Data->QuadVertexArray->Bind();
	}

	void Renderer3D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color)
	{
		DrawRotatedQuad({ position.x, position.y, 0.0f }, size, rotation, color);
	}

	void Renderer3D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& color)
	{
		HZ_PROFILE_FUNCTION();

		s_Data->TextureShader->SetFloat4("u_Color", color);
		s_Data->TextureShader->SetFloat("u_TilingFactor", 1.0f);
		s_Data->WhiteTexture->Bind(0);

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::rotate(glm::mat4(1.0f), rotation, { 0.0f, 0.0f, 1.0f })
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });
		s_Data->TextureShader->SetMat4("u_Transform", transform);
		s_Data->QuadVertexArray->Bind();
	}

	void Renderer3D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		DrawRotatedQuad({ position.x, position.y, 0.0f }, size, rotation, texture, tilingFactor, tintColor);
	}

	void Renderer3D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		HZ_PROFILE_FUNCTION();

		s_Data->TextureShader->SetFloat4("u_Color", tintColor);
		s_Data->TextureShader->SetFloat("u_TilingFactor", tilingFactor);
		texture->Bind(0);

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::rotate(glm::mat4(1.0f), rotation, { 0.0f, 0.0f, 1.0f })
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });
		s_Data->TextureShader->SetMat4("u_Transform", transform);

		
	}


	void Renderer3D::DrawPrimitives()
	{
		for (int i = 0; i < s_ObjData->size(); i++)
		{
			Renderer3D::Renderer3DStorage* curr = (*s_ObjData)[i];
			curr->TextureShader->Bind();
			curr->TextureShader->SetFloat4("u_Color", *curr->Color);
			curr->TextureShader->SetFloat("u_TilingFactor", 1.0f);
			curr->WhiteTexture->Bind(0);
			// transform 里面有 translate 和 scale了
			glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f))
				* glm::scale(glm::mat4(1.0f), *curr->Scale);
			curr->TextureShader->SetMat4("u_Transform", transform);
			curr->TextureShader->SetInt("u_Texture", 0);
			curr->TextureShader->SetMat4("u_ViewProjection", m_ViewProjection);
			curr->TextureShader->SetFloat3("u_CameraPos", m_CameraPos);
			curr->QuadVertexArray->Bind();
			switch (curr->DrawType)
			{
			case Renderer3D::DRAW_TYPE::HZ_TRIANGLES:
				RendererCommand::DrawIndexed(curr->QuadVertexArray);
				break;
			case Renderer3D::DRAW_TYPE::HZ_LINES:
				RendererCommand::DrawLines(curr->QuadVertexArray);
				break;
			default:
				RendererCommand::DrawIndexed(curr->QuadVertexArray);
				break;
			}
		}
	}

}
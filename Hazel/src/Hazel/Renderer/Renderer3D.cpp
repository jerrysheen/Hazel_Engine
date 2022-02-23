#include "hzpch.h"
#include "Renderer3D.h"


#include "RendererCommand.h"
#include "Renderer.h"

namespace Hazel {

	std::vector<Renderer3D::Renderer3DStorage*>* Renderer3D::s_ObjData = new std::vector<Renderer3D::Renderer3DStorage*>();
	glm::mat4  Renderer3D::m_ViewProjection;

	static Renderer3D::Renderer3DStorage* s_Data;

	void Renderer3D::Init()
	{
		
		s_ObjData = new std::vector<Renderer3D::Renderer3DStorage*>();
		s_Data = new Renderer3DStorage();

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
	}

	void Renderer3D::EndScene()
	{
	}

	void Renderer3D::CreatePlane()
	{
		CreatePlane({0.0f, 0.0f, 0.0f});
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

		s_Data->TextureShader = Shader::Create("assets/shaders/Texture.glsl");
		s_Data->TextureShader->Bind();
		s_Data->TextureShader->SetInt("u_Texture", 0);
		s_Data->TextureShader->SetMat4("u_ViewProjection", m_ViewProjection);

		s_ObjData->push_back(s_Data);
	}

	void Renderer3D::DrawPrimitives()
	{
		for (int i = 0; i < s_ObjData->size(); i++)
		{
			Renderer3D::Renderer3DStorage* curr = (*s_ObjData)[i];
			curr->TextureShader->Bind();
			curr->TextureShader->SetFloat4("u_Color", glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
			curr->TextureShader->SetFloat("u_TilingFactor", 1.0f);
			curr->WhiteTexture->Bind(0);
			glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f))
				* glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
			curr->TextureShader->SetMat4("u_Transform", transform);

			curr->QuadVertexArray->Bind();
			RendererCommand::DrawIndexed(curr->QuadVertexArray);
		}
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
		RendererCommand::DrawIndexed(s_Data->QuadVertexArray);
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
		RendererCommand::DrawIndexed(s_Data->QuadVertexArray);
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
		RendererCommand::DrawIndexed(s_Data->QuadVertexArray);
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

		
		RendererCommand::DrawIndexed(s_Data->QuadVertexArray);
	}

}
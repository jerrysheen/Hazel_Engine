#pragma once

#include "OrthographicCamera.h"
#include "PerspectiveCamera.h"
#include "glm/gtc/matrix_transform.hpp"
#include "Texture.h"
#include "hzpch.h"
#include "VertexArray.h"
#include "Shader.h"

namespace Hazel {
	
	class Renderer3D
	{
	public:
		static void Init();
		static void Shutdown();

		static void BeginScene(const OrthographicCamera& camera);
		static void BeginScene(const PerspectiveCamera& camera);

		static void EndScene();

		static void CreatePlane();
		static void CreatePlane(const glm::vec3& position);

		static void RenderGround();

		// Primitives
		static void DrawPrimitives();

		static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color);
		static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color);
		static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));
		static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));

		static void DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color);
		static void DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& color);
		static void DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));
		static void DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));

		enum class DRAW_TYPE
		{
			HZ_LINES, HZ_TRIANGLES
		};
		struct Renderer3DStorage
		{
			Ref<VertexArray> QuadVertexArray;
			Ref<Shader> TextureShader;
			Ref<Texture2D> WhiteTexture;
			Ref<glm::vec3> Scale;
			Ref<glm::vec4> Color;
			DRAW_TYPE DrawType;
		};
	private:
		inline static  float m_squareVertices[5 * 4] = {
			-0.5f, 0.0f, -0.5f, 0.0f, 0.0f,
			 0.5f, 0.0f, -0.5f, 1.0f, 0.0f,
			 0.5f,  0.0f, 0.5f, 1.0f, 1.0f,
			-0.5f,  0.0f, 0.5f, 0.0f, 1.0f
		};

		inline static  float m_ground[4 * 3] = {
			-10.f, 0.0f, 0.0f,
			10.f, 0.0f,  0.0f,
			0.0f, 0.0f, 10.0f,
			0.0f, 0.0f, -10.0f,
		};
		inline static uint32_t m_squareIndices[6] = { 0, 1, 2, 2, 3, 0 };
		
		inline static uint32_t m_groundIndices[4] = { 0, 1, 2, 3};
		static std::vector<Renderer3D::Renderer3DStorage*>* s_ObjData;
		static glm::mat4 m_ViewProjection;
		static glm::vec3 m_CameraPos;
	};

}

	
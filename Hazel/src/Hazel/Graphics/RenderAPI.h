#pragma once

#include <glm/glm.hpp>

#include "Hazel/Renderer/VertexArray.h"

namespace Hazel {
	
	class RenderAPI
	{
	public:
		enum class API
		{
			None = 0, OpenGL = 1, DirectX12 = 2
		};
	public:
		virtual void Init() = 0;
		virtual void SetViewPort(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;

		virtual void SetClearColor(const glm::vec4& color) = 0;
		virtual void Clear() = 0;

		virtual void DrawIndexed(const Ref<VertexArray>& vertexArray) = 0;
		virtual void DrawLines(const Ref<VertexArray>& vertexArray) = 0;
		
		inline static API GetAPI() { return s_API; };

	private:
		static API s_API;
	};
}
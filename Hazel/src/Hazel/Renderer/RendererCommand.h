#pragma once

#include "RendererAPI.h"
namespace Hazel {

	class RendererCommand
	{
	public:
		inline static void Init()
		{
			s_RendererAPI->Init();
		}

		inline static void SetViewPort(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
		{
			s_RendererAPI->SetViewPort(x, y, height, width);
		}
		inline static void SetClearColor(const glm::vec4& color)
		{
			s_RendererAPI->SetClearColor(color);
		}
		
		inline static void Clear()
		{
			s_RendererAPI->Clear();
		}

		inline static void DrawIndexed(const Ref<VertexArray>& vertexArray)
		{
			s_RendererAPI->DrawIndexed(vertexArray);
		}

		inline static void DrawLines(const Ref<VertexArray>& vertexArray) 
		{
			s_RendererAPI->DrawLines(vertexArray);
		}

	private:
		static RendererAPI* s_RendererAPI;
	};
}
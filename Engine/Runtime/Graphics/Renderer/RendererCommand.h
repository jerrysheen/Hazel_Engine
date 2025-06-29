#pragma once

#include "Runtime/Graphics/RenderAPI.h"
namespace Hazel {

	class RendererCommand
	{
	public:
		inline static void Init()
		{
			s_RenderAPI->Init();
		}

		inline static void SetViewPort(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
		{
			s_RenderAPI->SetViewPort(x, y, height, width);
		}
		inline static void SetClearColor(const glm::vec4& color)
		{
			s_RenderAPI->SetClearColor(color);
		}
		
		inline static void Clear()
		{
			s_RenderAPI->Clear();
		}

		inline static void DrawIndexed(const Ref<VertexArray>& vertexArray)
		{
			s_RenderAPI->DrawIndexed(vertexArray);
		}

		inline static void DrawLines(const Ref<VertexArray>& vertexArray) 
		{
			s_RenderAPI->DrawLines(vertexArray);
		}

	private:
		static RenderAPI* s_RenderAPI;
	};
}
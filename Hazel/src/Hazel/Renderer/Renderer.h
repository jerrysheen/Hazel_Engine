#pragma once

#include "RendererAPI.h"
namespace Hazel {
	
	//enum class RendererAPI
	//{
	//	None = 0,
	//	OpenGL = 1
	//};

	class Renderer 
	{
	public:
		static void BeginScene();
		static void EndScene();

		static void Submit(const std::shared_ptr<VertexArray>& vertexArray);

		inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); };
	};
}
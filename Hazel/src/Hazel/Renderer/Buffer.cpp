#include "hzpch.h"
#include "Buffer.h"

#include "Renderer.h"

#include "Platform/OpenGL/OpenGLBuffer.h"
namespace Hazel {
	VertexBuffer* VertexBuffer::Create(float* vertices, uint32_t size)
	{
		switch (Renderer::GetAPI())
		{
			case RenderAPI::API::None: HZ_CORE_ASSERT(false, "RenderAPI::None is currently not supported");
			case RenderAPI::API::OpenGL: return new OpenGLVertexBuffer(vertices, size);
		}
		HZ_CORE_ASSERT(false, "Unknowed API...");
		return nullptr;
	}

	
	IndexBuffer* IndexBuffer::Create(uint32_t* indices, uint32_t size)
	{
		switch (Renderer::GetAPI())
		{
			case RenderAPI::API::None: HZ_CORE_ASSERT(false, "RenderAPI::None is currently not supported");
			case RenderAPI::API::OpenGL: return new OpenGLIndexBuffer(indices, size);
		}
		HZ_CORE_ASSERT(false, "Unknowed API...");
		return nullptr;
	}

}


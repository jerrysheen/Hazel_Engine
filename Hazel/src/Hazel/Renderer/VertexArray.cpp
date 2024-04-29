#include "hzpch.h"
#include "VertexArray.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGLVertexArray.h"

namespace Hazel {

	Ref<VertexArray> VertexArray::Create()
	{
		switch (Renderer::GetAPI())
		{
			case RenderAPI::API::None: HZ_CORE_ASSERT(false, "RenderAPI::None is currently not supported");
			case RenderAPI::API::OpenGL: return std::make_shared<OpenGLVertexArray>();
		}
		HZ_CORE_ASSERT(false, "Unknowed API...");
		return nullptr;
	}


	void VertexArray::Bind() const
	{
	}

	void VertexArray::Unbind() const
	{
	}

	void VertexArray::AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer)
	{
	}

	void VertexArray::SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer)
	{
	}

}
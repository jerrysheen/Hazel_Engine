#include "hzpch.h"
#include "VertexArray.h"

#include "Hazel/Graphics/RenderAPI.h"
#include "Platform/OpenGL/OpenGLVertexArray.h"
#include "Platform/D3D12/D3D12VertexArray.h"

namespace Hazel {

	Ref<VertexArray> VertexArray::Create()
	{
		switch (RenderAPI::GetAPI())
		{
			case RenderAPI::API::None: HZ_CORE_ASSERT(false, "RenderAPI::None is currently not supported");
			case RenderAPI::API::OpenGL: return std::make_shared<OpenGLVertexArray>();
			case RenderAPI::API::DirectX12: return std::make_shared<D3D12VertexArray>();
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
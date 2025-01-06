#include "hzpch.h"
#include "Buffer.h"

#include "Hazel/Graphics/RenderAPI.h"

#include "Platform/OpenGL/OpenGLBuffer.h"
#include "Platform/D3D12/D3D12ConstantBuffer.h"
namespace Hazel {
	VertexBuffer* VertexBuffer::Create(float* vertices, uint32_t size)
	{
		switch (RenderAPI::GetAPI())
		{
			case RenderAPI::API::None: HZ_CORE_ASSERT(false, "RenderAPI::None is currently not supported");
			case RenderAPI::API::OpenGL: return new OpenGLVertexBuffer(vertices, size);
		}
		HZ_CORE_ASSERT(false, "Unknowed API...");
		return nullptr;
	}

	
	IndexBuffer* IndexBuffer::Create(uint32_t* indices, uint32_t size)
	{
		switch (RenderAPI::GetAPI())
		{
			case RenderAPI::API::None: HZ_CORE_ASSERT(false, "RenderAPI::None is currently not supported");
			case RenderAPI::API::OpenGL: return new OpenGLIndexBuffer(indices, size);
		}
		HZ_CORE_ASSERT(false, "Unknowed API...");
		return nullptr;
	}	

	// ����һ��ConstantBuffer, ��ʵֻ��Ҫ����һ��T���������T��size�Ϳ����ˣ�Ȼ���ڲ���һ��cpu�˵�buffer��

	Ref<ConstantBuffer> ConstantBuffer::Create(uint32_t bufferSize)
	{
		auto elementSize = 0;
		
		switch (RenderAPI::GetAPI())
		{
		case RenderAPI::API::DirectX12: return std::make_shared<D3D12ConstantBuffer>(bufferSize); break;
			default: HZ_CORE_ASSERT(false, "RenderAPI::None is currently not supported");
			//case RenderAPI::API::OpenGL: return new OpenGLIndexBuffer(size);
		}
		HZ_CORE_ASSERT(false, "Unknowed API...");
		return nullptr;
	}

}


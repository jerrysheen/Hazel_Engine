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

	// 创建一个ConstantBuffer, 其实只需要传入一个T，计算这个T的size就可以了，然后内部和一个cpu端的buffer绑定

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


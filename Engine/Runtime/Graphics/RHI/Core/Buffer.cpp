#include "hzpch.h"
#include "Buffer.h"

#include "Runtime/Graphics/RenderAPI.h"

#include "Platform/OpenGL/OpenGLBuffer.h"
#include "Platform/D3D12/D3D12Buffer.h"
#include "Platform/D3D12/D3D12ConstantBuffer.h"
namespace Hazel {
	Ref<VertexBuffer> VertexBuffer::Create(float* vertices, uint32_t size, uint32_t stride)
	{
		switch (RenderAPI::GetAPI())
		{
		case RenderAPI::API::None: HZ_CORE_ASSERT(false, "RenderAPI::None is currently not supported"); break;
		case RenderAPI::API::OpenGL: return std::make_shared<OpenGLVertexBuffer>(vertices, size, stride); break;
		case RenderAPI::API::DirectX12: return std::make_shared<D3D12VertexBuffer>(vertices, size, stride); break;

		}
		HZ_CORE_ASSERT(false, "Unknowed API...");
		return nullptr;
	}

	
	Ref<IndexBuffer> IndexBuffer::Create(uint16_t* indices, uint32_t size)
	{
		switch (RenderAPI::GetAPI())
		{
			case RenderAPI::API::None: HZ_CORE_ASSERT(false, "RenderAPI::None is currently not supported");
			case RenderAPI::API::DirectX12: return std::make_shared<D3D12IndexBuffer>(indices, size);
			case RenderAPI::API::OpenGL: return std::make_shared<OpenGLIndexBuffer>(indices, size);
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


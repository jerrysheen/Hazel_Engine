#include "hzpch.h"
#include "TextureBuffer.h"
#include "Platform/OpenGL/OpenGLFramebuffer.h"
#include "Platform/D3D12/D3D12TextureBuffer.h"
#include "Hazel/Graphics/RenderAPI.h"

namespace Hazel 
{
	Ref<TextureBuffer> TextureBuffer::Create(const TextureBufferSpecification& spec)
	{
		// 创建一个uuid给这个TextureBuffer
		

		switch (RenderAPI::GetAPI())
		{
			case RenderAPI::API::None: HZ_CORE_ASSERT(false, "RenderAPI::None is currently not supported");
			case RenderAPI::API::DirectX12: return CreateRef<D3D12TextureBuffer>(spec);
			case RenderAPI::API::OpenGL: return CreateRef<OpenGLFramebuffer>(spec);
		}
		HZ_CORE_ASSERT(false, "Unknowed API...");
		return nullptr;
	}


}
#include "hzpch.h"
#include "TextureBuffer.h"
#include "Platform/OpenGL/OpenGLFramebuffer.h"
#include "Hazel/Graphics/RenderAPI.h"

namespace Hazel 
{
	Ref<TextureBuffer> TextureBuffer::Create(const FramebufferSpecification& spec)
	{
		switch (RenderAPI::GetAPI())
		{
			case RenderAPI::API::None: HZ_CORE_ASSERT(false, "RenderAPI::None is currently not supported");
			//case RenderAPI::API::DirectX12: return CreateRef<DirectX12Framebuffer>(spec);
			case RenderAPI::API::OpenGL: return CreateRef<OpenGLFramebuffer>(spec);
		}
		HZ_CORE_ASSERT(false, "Unknowed API...");
		return nullptr;
	}
}
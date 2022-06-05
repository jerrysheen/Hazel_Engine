#include "hzpch.h"
#include "Framebuffer.h"
#include "Platform/OpenGL/OpenGLFramebuffer.h"
#include "Renderer.h"

namespace Hazel 
{
	Ref<Framebuffer> Framebuffer::Create(const FramebufferSpecification& spec)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None: HZ_CORE_ASSERT(false, "RendererAPI::None is currently not supported");
			case RendererAPI::API::OpenGL: return CreateRef<OpenGLFramebuffer>(spec);
		}
		HZ_CORE_ASSERT(false, "Unknowed API...");
		return nullptr;
	}
}
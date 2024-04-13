#include "hzpch.h"
#include "Hazel/Core/Window.h"
#include "Platform/Windows/WindowsGLFWWindow.h"

namespace Hazel
{
	Window* Window::Create(const WindowProps& props)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: HZ_CORE_ASSERT(false, "RendererAPI::None is currently not supported");
		case RendererAPI::API::OpenGL: return new WindowsGLFWWindow(props);;
		case RendererAPI::API::DirectX12: return nullptr;
		}
		return nullptr;
	}
}

#include "hzpch.h"
#include "Hazel/Core/Window.h"
#include "Platform/Windows/WindowsGLFWWindow.h"
#include "Platform/Windows/WindowsDXGIWindow.h"

namespace Hazel
{
	Window* Window::Create(const WindowProps& props)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: HZ_CORE_ASSERT(false, "RendererAPI::None is currently not supported"); break;
		case RendererAPI::API::OpenGL: return new WindowsGLFWWindow(props); break;
		case RendererAPI::API::DirectX12: return new WindowsDXGIWindow(props); break;
		}
		return nullptr;
	}
}

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
		case RenderAPI::API::None: HZ_CORE_ASSERT(false, "RenderAPI::None is currently not supported"); break;
		case RenderAPI::API::OpenGL: return new WindowsGLFWWindow(props); break;
		case RenderAPI::API::DirectX12: return new WindowsDXGIWindow(props); break;
		}
		return nullptr;
	}
}

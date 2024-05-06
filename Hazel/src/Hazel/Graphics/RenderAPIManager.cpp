#include "hzpch.h"
#include "Hazel/Graphics/RenderAPIManager.h"
#include "Hazel/Renderer/Renderer.h"
#include "Platform/D3D12/D3D12RenderAPIManager.h"
//#include "Platform/Windows/WindowsDXGIWindow.h"
namespace Hazel
{
	RenderAPIManager* RenderAPIManager::Create()
	{
		switch (Renderer::GetAPI())
		{
		case RenderAPI::API::None: HZ_CORE_ASSERT(false, "RenderAPI::None is currently not supported"); break;
		//case RenderAPI::API::OpenGL: return new WindowsGLFWWindow(props); break;
		case RenderAPI::API::DirectX12: return new D3D12RenderAPIManager(); break;
		}
		return nullptr;
	}
}

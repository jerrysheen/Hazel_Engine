#include "hzpch.h"
#include "platform/OpenGL/OpenGLRenderAPI.h"
#include "Platform/D3D12/D3D12RenderAPI.h"

#include "RendererCommand.h"

namespace Hazel {
	
	RenderAPI* RendererCommand::s_RenderAPI = [&]() -> RenderAPI* {
		switch (RenderAPI::GetAPI())
		{
			case RenderAPI::API::None:    return nullptr;
			case RenderAPI::API::OpenGL:  return new OpenGLRenderAPI();
			case RenderAPI::API::DirectX12:  return new D3D12RenderAPI();
        }
        }();
}
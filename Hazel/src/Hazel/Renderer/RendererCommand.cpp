#include "hzpch.h"
#include "platform/OpenGL/OpenGLRendererAPI.h"
#include "Platform/D3D12/D3D12RendererAPI.h"

#include "RendererCommand.h"

namespace Hazel {
	
	RendererAPI* RendererCommand::s_RendererAPI = [&]() -> RendererAPI* {
		switch (RendererAPI::GetAPI())
		{
			case RendererAPI::API::None:    return nullptr;
			case RendererAPI::API::OpenGL:  return new OpenGLRendererAPI();
			case RendererAPI::API::DirectX12:  return new D3D12RendererAPI();
        }
        }();
}
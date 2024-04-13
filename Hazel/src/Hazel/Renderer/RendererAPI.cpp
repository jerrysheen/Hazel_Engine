#include "hzpch.h"
#include "RendererAPI.h"

namespace Hazel {
#ifdef RENDER_API_OPENGL
	RendererAPI::API RendererAPI::s_API = RendererAPI::API::OpenGL;
#elif RENDER_API_DIRECTX12 // RENDER_API_OPENGL
	RendererAPI::API RendererAPI::s_API = RendererAPI::API::DirectX12;
#endif // DEBUG

}
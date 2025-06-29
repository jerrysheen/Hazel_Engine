#include "hzpch.h"
#include "RenderAPI.h"

namespace Hazel {
#ifdef RENDER_API_OPENGL
	RenderAPI::API RenderAPI::s_API = RenderAPI::API::OpenGL;
#elif RENDER_API_DIRECTX12 // RENDER_API_OPENGL
	RenderAPI::API RenderAPI::s_API = RenderAPI::API::DirectX12;
#endif // DEBUG

}
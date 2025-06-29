#include "RenderTarget.h"
#include "hzpch.h"
#include "Platform/OpenGL/OpenGLFramebuffer.h"
#include "Platform/D3D12/D3D12RenderTarget.h"
#include "Runtime/Graphics/RenderAPI.h"

namespace Hazel
{
	Ref<RenderTarget> RenderTarget::Create(const RenderTargetDescriptor& spec)
	{

		switch (RenderAPI::GetAPI())
		{
		case RenderAPI::API::None: HZ_CORE_ASSERT(false, "RenderAPI::None is currently not supported");
		case RenderAPI::API::DirectX12: return CreateRef<D3D12RenderTarget>(spec);
		//case RenderAPI::API::OpenGL: return CreateRef<OpenGLFramebuffer>(spec);
		}
		HZ_CORE_ASSERT(false, "Unknowed API...");
		return nullptr;
	}
}
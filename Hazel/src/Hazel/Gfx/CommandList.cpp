#include "hzpch.h"
#include "CommandList.h"
#include "Hazel/Graphics/RenderAPI.h"
#include "Platform/D3D12/D3D12CommandList.h"

namespace Hazel
{
	Ref<CommandList> CommandList::Create()
	{
		switch(RenderAPI::GetAPI()) {
		case RenderAPI::API::None: HZ_CORE_ASSERT(false, "RenderAPI::None is currently not supported");
		case RenderAPI::API::DirectX12: return std::make_shared<D3D12CommandList>();
			//case RenderAPI::API::OpenGL: return CreateRef<OpenGLFramebuffer>(spec);
		}
		HZ_CORE_ASSERT(false, "Unknowed API...");
		return nullptr;
	}
}
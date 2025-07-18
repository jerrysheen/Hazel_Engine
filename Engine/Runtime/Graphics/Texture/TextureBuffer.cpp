#include "hzpch.h"
#include "TextureBuffer.h"
#include "Platform/OpenGL/OpenGLFramebuffer.h"
#include "Platform/D3D12/D3D12TextureBuffer.h"
#include "Runtime/Graphics/RenderAPI.h"

namespace Hazel 
{

	Ref<TextureBuffer> TextureBuffer::Create(const TextureBufferSpecification& spec)
	{
		// ����һ��uuid�����TextureBuffer

		switch (RenderAPI::GetAPI())
		{
		case RenderAPI::API::None: HZ_CORE_ASSERT(false, "RenderAPI::None is currently not supported");
		case RenderAPI::API::DirectX12: return  std::make_shared<D3D12TextureBuffer>(spec);
		case RenderAPI::API::OpenGL: return  std::make_shared<OpenGLFramebuffer>(spec);
		}
		HZ_CORE_ASSERT(false, "Unknowed API...");
		return nullptr;
	}

}
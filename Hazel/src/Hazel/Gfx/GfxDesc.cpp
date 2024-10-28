#include "GfxDesc.h"
#include "hzpch.h"
#include "Hazel/Graphics/RenderAPI.h"
#include "Platform/D3D12/D3D12GfxDescHeap.h"

namespace Hazel
{

	Ref<GfxDescHeap> GfxDescHeap::Create(const DescriptorType& type)
	{
		switch (RenderAPI::GetAPI()) {
		case RenderAPI::API::None: HZ_CORE_ASSERT(false, "RenderAPI::None is currently not supported");
			break;
		case RenderAPI::API::DirectX12: return std::make_shared<D3D12GfxDescHeap>(type);
			//case RenderAPI::API::OpenGL: return CreateRef<OpenGLFramebuffer>(spec);
		}
		HZ_CORE_ASSERT(false, "Unknowed API...");
		return nullptr;
	}

	//Ref<GfxDesc> GfxDesc::Create(const DescriptorType& type)
	//{
	//	switch (RenderAPI::GetAPI()) {
	//	case RenderAPI::API::None: HZ_CORE_ASSERT(false, "RenderAPI::None is currently not supported");
	//		break;
	//	case RenderAPI::API::DirectX12: return std::make_shared<D3D12GfxDesc>(type);
	//		//case RenderAPI::API::OpenGL: return CreateRef<OpenGLFramebuffer>(spec);
	//	}
	//	HZ_CORE_ASSERT(false, "Unknowed API...");
	//	return nullptr;
	//}

	GfxDesc::GfxDesc()
	{
		m_CPUHandle = D3D12_CPU_DESCRIPTOR_HANDLE{};
		m_GPUHandle = D3D12_GPU_DESCRIPTOR_HANDLE{};
	}

}

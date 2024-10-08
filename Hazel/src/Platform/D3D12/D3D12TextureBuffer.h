#pragma once
#include "Hazel/Renderer/TextureBuffer.h"
#include "Platform/D3D12/d3dx12.h"
#include "Platform/D3D12/d3dUtil.h"
#include "Platform/D3D12/D3D12RenderAPIManager.h"
namespace Hazel 
{
	class D3D12TextureBuffer : public TextureBuffer
	{
	public:
		D3D12TextureBuffer() = default;
		D3D12TextureBuffer(const TextureBufferSpecification& spec);
		virtual ~D3D12TextureBuffer();
		void CreateBufferResource();
		void CreateBufferDesc();
	private:
		TextureBufferSpecification m_Spec;
		uint32_t m_RendererID;
	private:
		D3D12_RESOURCE_DIMENSION GetResourceDimension();
		DXGI_FORMAT GetTextureFormat();
		int GetMSAASamplerCount();
		int GetMipMapLevel();
	};

}
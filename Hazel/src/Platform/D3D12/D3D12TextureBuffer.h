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
		//D3D12TextureBuffer() = default;
		D3D12TextureBuffer(const TextureBufferSpecification& spec);
		virtual ~D3D12TextureBuffer();
		void CreateBufferResource();

		virtual void Bind() override {};
		virtual void Unbind() override {};
		virtual void RebindColorAttachment(uint32_t colorAttachmentID, TextureBufferSpecification spec) override {};
		virtual void RebindDepthAttachment(uint32_t depthAttachmentID, TextureBufferSpecification spec) override {};
		virtual void RebindColorAndDepthAttachment(uint32_t colorAttachmentID, uint32_t depthAttachmentID, TextureBufferSpecification spec) override {};

		virtual void Resize(const glm::vec2& viewportSize) override {};
		virtual uint32_t GetColorAttachmentRendererID() const override { return 1; };
		virtual uint32_t GetDepthAttachmentRendererID() const override { return 1;};
		virtual std::any GetRendererID() const override { return 1; };
		

		virtual const TextureBufferSpecification& GetSpecification() const override { return m_Spec; };
	private:
		TextureBufferSpecification m_Spec;
		Microsoft::WRL::ComPtr<ID3D12Resource> m_BufferResourceLocal;
	private:
		D3D12_RESOURCE_DIMENSION GetResourceDimension();
		DXGI_FORMAT GetTextureFormat();
		int GetMSAASamplerCount();
		int GetMipMapLevel();
	};

}
#pragma once
#include "Runtime/Graphics/Renderer/Texture.h"
#include <d3d12.h>
namespace Hazel 
{
	class D3D12Texture2D : public Texture2D 
	{
	public:

		//OpenGLTexture2D(uint32_t width, uint32_t height, uint32_t colorFormat);
		D3D12Texture2D(const std::string& path);
		//OpenGLTexture2D(const std::string& path, bool isCompressedImage, bool enableMip);
		~D3D12Texture2D();

		virtual uint32_t GetWidth() const override { return m_Width; }
		virtual uint32_t GetHeight() const override { return m_Height; }
		virtual uint32_t GetRendererID() const override { return m_RendererID; }

		virtual void SetData(void* data, uint32_t size) const override;

		virtual void Bind(uint32_t slot) const override;

	private:
		ID3D12Resource* uploadHeap;
		std::string m_Path;
		uint32_t m_Width;
		uint32_t m_Height;
		uint32_t m_RendererID;
	
	};
}
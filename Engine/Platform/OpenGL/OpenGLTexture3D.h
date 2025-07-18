#pragma once

#include "Runtime/Graphics/Texture/Texture.h"
#include <glad/glad.h>
namespace Hazel {

	class OpenGLTexture3D : public Texture3D
	{
	public:

		OpenGLTexture3D(uint32_t width, uint32_t height);
		OpenGLTexture3D(const std::vector<std::string>& path);
		OpenGLTexture3D(const std::vector<std::string>& path, bool isCompressed, bool enableMipMap);
		~OpenGLTexture3D();

		virtual uint32_t GetWidth() const override { return m_Width; }
		virtual uint32_t GetHeight() const override { return m_Height; }
		virtual uint32_t GetRendererID() const override { return m_RendererID; }

		virtual void SetData(void* data, uint32_t size) const override { return; };

		virtual void Bind(uint32_t slot) const override;

	private:
		std::string m_Path;
		uint32_t m_Width;
		uint32_t m_Height;
		uint32_t m_RendererID;

		GLenum m_InternalFormat, m_DataFormat;

		std::vector<Ref<Texture2D>> faces;
		//"right.jpg",
		//	"left.jpg",
		//	"top.jpg",
		//	"bottom.jpg",
		//	"front.jpg",
		//	"back.jpg"
	};

}
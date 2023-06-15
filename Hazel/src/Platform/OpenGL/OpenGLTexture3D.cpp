#include "hzpch.h"
#include "OpenGLTexture3D.h"

#include "stb_image.h"

namespace Hazel 
{
	OpenGLTexture3D::OpenGLTexture3D(uint32_t width, uint32_t height)
	{
	}

	OpenGLTexture3D::OpenGLTexture3D(const std::vector<std::string>& path)
	{
		// get the path first:
		for each (std::string var in path)
		{
			HZ_CORE_INFO("{0}", var);
		}
	}

	OpenGLTexture3D::OpenGLTexture3D(const std::vector<std::string>& paths, bool isCompressedImage, bool enableMip)
	{

		glGenTextures(1, &m_RendererID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererID);
		
		for (int i = 0; i < paths.size(); i++)
		{
			//faces.push_back(Texture2D::Create(var, true, true));

			int width, height, chanels;
			stbi_set_flip_vertically_on_load(0);
			stbi_uc* data = stbi_load(paths[i].c_str(), &width, &height, &chanels, 0);
			HZ_CORE_ASSERT(data, "Failed to load the image");

			//HZ_CORE_INFO("image info Height: {0}, Width: {1}", width, height);
			GLenum internalFormat = 0, dataFormat = 0;
			if (chanels == 4)
			{
				internalFormat = GL_RGBA8;
				dataFormat = GL_RGBA;
			}
			else if (chanels == 3)
			{
				internalFormat = GL_RGB8;
				dataFormat = GL_RGB;
			}

			HZ_CORE_ASSERT(dataFormat && internalFormat, "Unsupported image chanel.");

			//glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
			//glTextureStorage2D(m_RendererID, 1, internalFormat, m_Width, m_Height);

			//glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			//glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	}

	OpenGLTexture3D::~OpenGLTexture3D()
	{
	}

	void OpenGLTexture3D::Bind(uint32_t slot) const
	{
		glBindTextureUnit(slot, m_RendererID);
	}
}

#include "hzpch.h"
#include "OpenGLTexture3D.h"

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

	OpenGLTexture3D::OpenGLTexture3D(const std::vector<std::string>& path, bool isCompressedImage, bool enableMip)
	{
		for each (std::string var in path)
		{
			faces.push_back(Texture2D::Create(var, true, true));
		}

		// just for fast test function
		for each (Ref<Texture2D> face in faces)
		{
			int rendererID = face->GetRendererID();
			
			glBindTexture(GL_TEXTURE_2D, rendererID);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
	}

	OpenGLTexture3D::~OpenGLTexture3D()
	{
	}

	void OpenGLTexture3D::Bind(uint32_t slot) const
	{
	}
}

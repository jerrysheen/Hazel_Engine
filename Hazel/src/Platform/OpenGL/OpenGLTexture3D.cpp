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

	OpenGLTexture3D::OpenGLTexture3D(const std::string& path, bool isCompressedImage)
	{
	}

	OpenGLTexture3D::~OpenGLTexture3D()
	{
	}

	void OpenGLTexture3D::Bind(uint32_t slot) const
	{
	}
}

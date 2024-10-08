#pragma once
#include "glm/gtc/type_ptr.hpp"

namespace Hazel 
{
	enum MultiSample
	{
		NONE,
		MSAA2X,
		MSAA4X,
		MSAA8X,
		MSAA16X
	};

	enum TextureType
	{
		TEXTURE2D,
		TEXTURECUBE,
		TEXTURE2DARRAY
	};

	enum TextureFormat
	{
		RGBA32,
		DEPTH24STENCIL8
	};

	enum TextureRenderUsage
	{
		RENDER_TARGET,
		RENDER_TEXTURE
	};

	struct TextureBufferSpecification
	{
		uint32_t width, height;
		TextureType textureType;
		TextureFormat format;
		TextureRenderUsage textureRenderUsage;
		MultiSample multiSample;
		TextureBufferSpecification() = default;

		TextureBufferSpecification(uint32_t width, uint32_t height, TextureType textureType = TEXTURE2D, TextureFormat format = RGBA32, TextureRenderUsage textureRenderUsage = RENDER_TEXTURE, MultiSample multiSample = NONE)
			: width(width), height(height), textureType(textureType), format(format), textureRenderUsage(textureRenderUsage), multiSample(multiSample) {}
	};
}
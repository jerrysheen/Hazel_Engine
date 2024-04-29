#include "hzpch.h"


#include "Texture.h"
#include "Renderer.h"
#include "Platform/OpenGL/OpenGLTexture2D.h"
#include "Platform/OpenGL/OpenGLTexture3D.h"

namespace Hazel {
	Ref<Texture2D> Texture2D::Create(uint32_t width, uint32_t height, uint32_t colorFormat)
	{
		switch (Renderer::GetAPI())
		{
		case RenderAPI::API::None:    HZ_CORE_ASSERT(false, "RenderAPI::None is currently not supported!"); return nullptr;
		case RenderAPI::API::OpenGL:  return  std::make_shared<OpenGLTexture2D>(width, height, colorFormat);
		}

		HZ_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;
	}


	Ref<Texture2D> Texture2D::Create(const std::string& path) 
	{
		switch (Renderer::GetAPI())
		{
			case RenderAPI::API::None: HZ_CORE_ASSERT(false, "RenderAPI::None is currently not supported");
			case RenderAPI::API::OpenGL: return std::make_shared<OpenGLTexture2D>(path);
		}
		HZ_CORE_ASSERT(false, "Unknowed API...");
		return nullptr;
	}

	Ref<Texture2D>  Texture2D::Create(const std::string& path, bool isCompressedImage, bool enableMip)
	{
		switch (Renderer::GetAPI())
		{
		case RenderAPI::API::None: HZ_CORE_ASSERT(false, "RenderAPI::None is currently not supported");
		case RenderAPI::API::OpenGL: return std::make_shared<OpenGLTexture2D>(path, isCompressedImage, enableMip);
		}
		HZ_CORE_ASSERT(false, "Unknowed API...");
		return nullptr;
	}


	Ref<Texture3D> Texture3D::Create(uint32_t width, uint32_t height)
	{
		switch (Renderer::GetAPI())
		{
		case RenderAPI::API::None:    HZ_CORE_ASSERT(false, "RenderAPI::None is currently not supported!"); return nullptr;
		case RenderAPI::API::OpenGL:  return  std::make_shared<OpenGLTexture3D>(width, height);
		}

		HZ_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;
	}


	Ref<Texture3D> Texture3D::Create(const std::vector<std::string>& path)
	{
		switch (Renderer::GetAPI())
		{
		case RenderAPI::API::None: HZ_CORE_ASSERT(false, "RenderAPI::None is currently not supported");
		case RenderAPI::API::OpenGL: return std::make_shared<OpenGLTexture3D>(path);
		}
		HZ_CORE_ASSERT(false, "Unknowed API...");
		return nullptr;
	}

	Ref<Texture3D>  Texture3D::Create(const std::vector<std::string>& path, bool isCompressedImage = true, bool enableMipMap = true)
	{
		switch (Renderer::GetAPI())
		{
		case RenderAPI::API::None: HZ_CORE_ASSERT(false, "RenderAPI::None is currently not supported");
		case RenderAPI::API::OpenGL: return std::make_shared<OpenGLTexture3D>(path, isCompressedImage, enableMipMap);
		}
		HZ_CORE_ASSERT(false, "Unknowed API...");
		return nullptr;
	}
}

#pragma once

namespace Hazel {

	class Texture 
	{
	public:
		virtual ~Texture() = default;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		virtual uint32_t GetRendererID() const = 0;
		virtual void SetData(void* data, uint32_t size) const = 0;

		virtual void Bind(uint32_t slot) const = 0;
	};

	class Texture2D : public Texture 
	{
	public:
		static Ref<Texture2D>  Create(const std::string& path);
		static Ref<Texture2D>  Create(const std::string& path, bool isCompressedImage, bool enableMip = true);
		static Ref<Texture2D>  Create(uint32_t width, uint32_t height, uint32_t colorFormat);
	};

	class Texture3D : public Texture
	{
	public:
		static Ref<Texture3D>  Create(const std::vector<std::string>& path);
		static Ref<Texture3D>  Create(const std::vector<std::string>& path, bool isCompressed, bool enableMip);
		static Ref<Texture3D>  Create(uint32_t width, uint32_t height);
	};
}
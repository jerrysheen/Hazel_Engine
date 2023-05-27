#include "hzpch.h"
#include "OpenGLTexture2D.h"

#include "stb_image.h"


namespace Hazel {

	OpenGLTexture2D::OpenGLTexture2D(uint32_t width, uint32_t height)
		: m_Width(width), m_Height(height)
	{
		m_InternalFormat = GL_RGBA8;
		m_DataFormat = GL_RGBA;

		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
		glTextureStorage2D(m_RendererID, 1, m_InternalFormat, m_Width, m_Height);

		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}

	OpenGLTexture2D::OpenGLTexture2D(const std::string& path)
		:m_Path(path)
	{
		int width, height, chanels;
		stbi_set_flip_vertically_on_load(1);
		stbi_uc* data =  stbi_load(path.c_str(), &width, &height, &chanels, 0);
		HZ_CORE_ASSERT(data, "Failed to load the image");

		m_Width = width;
		m_Height = height;
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

		m_InternalFormat = internalFormat;
		m_DataFormat = dataFormat;

		HZ_CORE_ASSERT(dataFormat && internalFormat, "Unsupported image chanel.");

		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
		glTextureStorage2D(m_RendererID, 1, internalFormat, m_Width, m_Height);

		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, dataFormat, GL_UNSIGNED_BYTE, data);

		stbi_image_free(data);
	}
	
	OpenGLTexture2D::OpenGLTexture2D(const std::string& path, bool isCompressedImage)
		:m_Path(path)
	{
		if (!isCompressedImage) 
		{
			OpenGLTexture2D::OpenGLTexture2D(path);
			HZ_CORE_ASSERT(false, "Only For Test");
			return;
		}
		// https://gist.github.com/tilkinsc/13191c0c1e5d6b25fbe79bbd2288a673
		// https://github.com/Dav1dde/glad/issues/194
		// lay out variables to be used
		unsigned char* header;

		unsigned int width;
		unsigned int height;
		unsigned int mipMapCount;

		unsigned int blockSize;
		unsigned int format;

		unsigned int w;
		unsigned int h;

		unsigned char* buffer = 0;

		// open the DDS file for binary reading and get file size
		FILE* f;
		if ((f = fopen(path.c_str(), "rb")) == 0)
		{
			HZ_CORE_ASSERT(false, "Can't Open File!!");
			return;
		}
		fseek(f, 0, SEEK_END);
		long file_size = ftell(f);
		fseek(f, 0, SEEK_SET);

		// allocate new unsigned char space with 4 (file code) + 124 (header size) bytes
		// read in 128 bytes from the file
		header = (unsigned char*)malloc(128 * sizeof(unsigned char));
		fread(header, 1, 128, f);

		// compare the `DDS ` signature
		if (memcmp(header, "DDS ", 4) != 0)
			goto exit;

		// extract height, width, and amount of mipmaps - yes it is stored height then width
		height = (header[12]) | (header[13] << 8) | (header[14] << 16) | (header[15] << 24);
		width = (header[16]) | (header[17] << 8) | (header[18] << 16) | (header[19] << 24);
		mipMapCount = (header[28]) | (header[29] << 8) | (header[30] << 16) | (header[31] << 24);

		// figure out what format to use for what fourCC file type it is
		// block size is about physical chunk storage of compressed data in file (important)
		if (header[84] == 'D') {
			switch (header[87]) {
			case '1': // DXT1
				format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
				blockSize = 8;
				break;
			case '3': // DXT3
				format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
				blockSize = 16;
				break;
			case '5': // DXT5
				format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
				blockSize = 16;
				break;
			case '0': // DX10
				// unsupported, else will error
				// as it adds sizeof(struct DDS_HEADER_DXT10) between pixels
				// so, buffer = malloc((file_size - 128) - sizeof(struct DDS_HEADER_DXT10));
			default: goto exit;
			}
		}
		else // BC4U/BC4S/ATI2/BC55/R8G8_B8G8/G8R8_G8B8/UYVY-packed/YUY2-packed unsupported
			goto exit;

		// allocate new unsigned char space with file_size - (file_code + header_size) magnitude
		// read rest of file
		buffer = (unsigned char*)malloc((file_size - 128) * sizeof(unsigned char));
		if (buffer == 0)
			goto exit;
		fread(buffer, 1, file_size, f);

		// prepare new incomplete texture
		//glGenTextures(1, &tid);
		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
		HZ_CORE_INFO("tid: {0}", m_RendererID);
		/*if (tid == 0)
			goto exit;*/

		// bind the texture
		// make it complete by specifying all needed parameters and ensuring all mipmaps are filled
		glBindTexture(GL_TEXTURE_2D, m_RendererID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, mipMapCount - 1); // opengl likes array length of mipmaps
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // don't forget to enable mipmaping
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		// prepare some variables
		unsigned int offset = 0;
		unsigned int size = 0;
		w = width;
		h = height;

		// loop through sending block at a time with the magic formula
		// upload to opengl properly, note the offset transverses the pointer
		// assumes each mipmap is 1/2 the size of the previous mipmap
		for (unsigned int i = 0; i < mipMapCount; i++) {
			if (w == 0 || h == 0) { // discard any odd mipmaps 0x1 0x2 resolutions
				mipMapCount--;
				continue;
			}
			size = ((w + 3) / 4) * ((h + 3) / 4) * blockSize;
			glCompressedTexImage2D(GL_TEXTURE_2D, i, format, w, h, 0, size, buffer + offset);
			offset += size;
			w /= 2;
			h /= 2;
		}
		// discard any odd mipmaps, ensure a complete texture
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, mipMapCount - 1);
		// unbind
		glBindTexture(GL_TEXTURE_2D, 0);

		// easy macro to get out quick and uniform (minus like 15 lines of bulk)
	exit:
		free(buffer);
		free(header);
		fclose(f);
		//m_RendererID = tid;
		//return tid;
	}
	OpenGLTexture2D::~OpenGLTexture2D()
	{
		glDeleteTextures(1, &m_RendererID);
	}

	void OpenGLTexture2D::SetData(void* data, uint32_t size) const
	{
		uint32_t bpp = m_DataFormat == GL_RGBA ? 4 : 3;
		HZ_CORE_INFO("{0}, {1}" ,m_Width, m_Height);
		HZ_CORE_ASSERT(size == m_Width * m_Height * bpp, "Data must be entire texture!");
		glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, m_DataFormat, GL_UNSIGNED_BYTE, data);
	}


	void OpenGLTexture2D::Bind(uint32_t slot) const
	{
		glBindTextureUnit(slot, m_RendererID);
	}
}
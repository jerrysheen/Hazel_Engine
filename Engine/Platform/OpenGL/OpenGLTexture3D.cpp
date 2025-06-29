#include "hzpch.h"
#include "OpenGLTexture3D.h"

#include "stb_image.h"

namespace Hazel 
{
	OpenGLTexture3D::OpenGLTexture3D(uint32_t width, uint32_t height)
	{
		glGenTextures(1, &m_RendererID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererID);
		for (unsigned int i = 0; i < 6; ++i)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, nullptr);
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
		return;
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
		if (!isCompressedImage)
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
			return;
		}

		HZ_CORE_INFO("Read compressed 3D Textures");
		glGenTextures(1, &m_RendererID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererID);

		for (int i = 0; i < paths.size(); i++)
		{
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
			if ((f = fopen(paths[i].c_str(), "rb")) == 0)
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

			mipMapCount = enableMip ? mipMapCount : 1;
			// figure out what format to use for what fourCC file type it is
			// block size is about physical chunk storage of compressed data in file (important)
			if (header[84] == 'D') {
				switch (header[87]) {
				case '1': // DXT1
					HZ_CORE_INFO("DXT1_Format");
					format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
					blockSize = 8;
					break;
				case '3': // DXT3
					HZ_CORE_INFO("DXT3_Format");
					format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
					blockSize = 16;
					break;
				case '5': // DXT5
					HZ_CORE_INFO("DXT5_Format");
					format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
					blockSize = 16;
					break;
				case '0': // DX10
					HZ_CORE_ASSERT(false, "Can't Open File!!");
					// unsupported, else will error
					// as it adds sizeof(struct DDS_HEADER_DXT10) between pixels
					// so, buffer = malloc((file_size - 128) - sizeof(struct DDS_HEADER_DXT10));
				default: 
				{
					HZ_CORE_ASSERT(false, "Can't Open File!!");
					goto exit;
				}
				}
			}
			else // BC4U/BC4S/ATI2/BC55/R8G8_B8G8/G8R8_G8B8/UYVY-packed/YUY2-packed unsupported
			{
				HZ_CORE_ASSERT(false, "Can't Open File!!");
				goto exit;
			}

			// allocate new unsigned char space with file_size - (file_code + header_size) magnitude
			// read rest of file
			buffer = (unsigned char*)malloc((file_size - 128) * sizeof(unsigned char));
			if (buffer == 0) 
			{
				HZ_CORE_ASSERT(false, "Can't Open File!!");
				goto exit;
			}
			fread(buffer, 1, file_size, f);


			// prepare some variables
			unsigned int offset = 0;
			unsigned int size = 0;
			w = width;
			h = height;

			// loop through sending block at a time with the magic formula
			// upload to opengl properly, note the offset transverses the pointer
			// assumes each mipmap is 1/2 the size of the previous mipmap
			for (unsigned int j = 0; j < mipMapCount; j++) {
				if (w == 0 || h == 0) { // discard any odd mipmaps 0x1 0x2 resolutions
					mipMapCount--;
					continue;
				}
				size = ((w + 3) / 4) * ((h + 3) / 4) * blockSize;
				glCompressedTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, j, format, w, h, 0, size, buffer + offset);
				offset += size;
				w /= 2;
				h /= 2;
			}
			// discard any odd mipmaps, ensure a complete texture
			// easy macro to get out quick and uniform (minus like 15 lines of bulk)
		exit:
			free(buffer);
			free(header);
			fclose(f);
			//m_RendererID = tid;
			//return tid;
		}

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
		return;




	}

	OpenGLTexture3D::~OpenGLTexture3D()
	{
	}

	void OpenGLTexture3D::Bind(uint32_t slot) const
	{
		glBindTextureUnit(slot, m_RendererID);
	}
}

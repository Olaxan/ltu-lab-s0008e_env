#include "tex_res.h"
#include "stb_image.h"

#include <GL/glew.h>
#include <iostream>

namespace efiilj
{
	TextureResource::TextureResource() : tex_id(0), width(0), height(0), bits_per_pixel(0) { }

	TextureResource::TextureResource(const char* path, bool flip) : tex_id(0), width(0), height(0), bits_per_pixel(0)
	{
		glGenTextures(1, &tex_id);
		glBindTexture(GL_TEXTURE_2D, tex_id);

		stbi_set_flip_vertically_on_load(flip);
		unsigned char* buffer = stbi_load(path, &width, &height, &bits_per_pixel, 4);

		std::cout << "Loaded texture " << tex_id << ": " << width << "x" << height << " / " << bits_per_pixel << "\n\n";

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
		glGenerateMipmap(GL_TEXTURE_2D);

		Unbind();
	}

	void TextureResource::Bind(unsigned int slot)
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, tex_id);
	}

	void TextureResource::Unbind()
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	TextureResource::~TextureResource()
	{
		std::cout << "Deleting texture resource " << this << std::endl;

		Unbind();
		glDeleteTextures(1, &tex_id);
	}
}

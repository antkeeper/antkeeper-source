/*
 * Copyright (C) 2017-2019  Christopher J. Howard
 *
 * This file is part of Antkeeper Source Code.
 *
 * Antkeeper Source Code is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Antkeeper Source Code is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Antkeeper Source Code.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "resource-loader.hpp"
#include "image.hpp"
#include <sstream>

#include <emergent/emergent.hpp>
using namespace Emergent;

template <>
Texture2D* ResourceLoader<Texture2D>::load(ResourceManager* resourceManager, std::istream* is)
{
	Image* image = ResourceLoader<Image>::load(resourceManager, is);

	// Generate OpenGL texture ID
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	// Set wrapping and filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	// Select texture formats according to number of color channels in the image
	GLint internalFormat;
	GLenum format;
	GLenum type = (image->isHDR()) ? GL_FLOAT : GL_UNSIGNED_BYTE;
	if (image->getChannels() == 1)
	{
		// Grey
		internalFormat = (image->isHDR()) ? GL_R32F : GL_R8;
		format = GL_RED;
		
		GLint swizzleMask[] = {GL_RED, GL_RED, GL_RED, GL_ONE};
		glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzleMask);
	}
	else if (image->getChannels() == 2)
	{
		// Grey, alpha
		internalFormat = (image->isHDR()) ? GL_RG32F : GL_RG8;
		format = GL_RG;
		
		GLint swizzleMask[] = {GL_RED, GL_RED, GL_RED, GL_GREEN};
		glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzleMask);
	}
	else if (image->getChannels() == 3)
	{
		// Red, green, blue
		internalFormat = (image->isHDR()) ? GL_RGB32F : GL_RGB8;
		format = GL_RGB;
		
		GLint swizzleMask[] = {GL_RED, GL_GREEN, GL_BLUE, GL_ONE};
		glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzleMask);
	}
	else if (image->getChannels() == 4)
	{
		// Red, green, blue, alpha
		internalFormat = (image->isHDR()) ? GL_RGBA32F : GL_RGBA8;
		format = GL_RGBA;
	}
	else
	{
		std::stringstream stream;
		stream << std::string("Texture cannot be created from an image with an unsupported number of color channels (") << image->getChannels() << std::string(").");
		delete image;
		glDeleteTextures(1, &textureID);
		throw std::runtime_error(stream.str().c_str());
	}

	// Upload image data to OpenGL
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, image->getWidth(), image->getHeight(), 0, format, type, image->getPixels());
	
	// Generate mipmaps
	glGenerateMipmap(GL_TEXTURE_2D);

	// Create Texture2D
	Texture2D* texture = new Texture2D();
	texture->setTextureID(textureID);
	texture->setWidth(image->getWidth());
	texture->setHeight(image->getHeight());

	// Free loaded image
	delete image;

	return texture;
}


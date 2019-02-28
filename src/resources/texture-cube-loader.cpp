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
#include "resource-manager.hpp"
#include "text-file.hpp"
#include <sstream>
#include <stdexcept>

#include <emergent/emergent.hpp>
using namespace Emergent;

namespace CubemapLayout
{
	enum
	{
		VERTICAL_CROSS,
		HORIZONTAL_CROSS,
		VERTICAL_STRIP,
		HORIZONTAL_STRIP,
		BLENDER
	};
}

static const int CUBEMAP_LAYOUT_DIMENSIONS[5][2] = 
{
	{3, 4}, // Vertical cross
	{4, 3}, // Horizontal cross
	{1, 6}, // Vertical strip
	{6, 1}, // Horizontal strip
	{3, 2}  // Blender
};

static const int CUBEMAP_LAYOUT_OFFSETS[5][6][2] = 
{
	// Vertical cross
	{
		{0, 1}, // +X
		{2, 1}, // -X
		{1, 0}, // +Y
		{1, 2}, // -Y
		{1, 1}, // +Z
		{1, 3}  // -Z
	},
	
	// Horizontal cross
	{
		{0, 1}, // +X
		{2, 1}, // -X
		{1, 0}, // +Y
		{1, 2}, // -Y
		{1, 1}, // +Z
		{3, 1}  // -Z
	},
	
	// Vertical strip
	{
		{0, 1}, // +X
		{0, 0}, // -X
		{0, 2}, // +Y
		{0, 3}, // -Y
		{0, 4}, // +Z
		{0, 5}  // -Z
	},
	
	// Horizontal strip
	{
		{1, 0}, // +X
		{0, 0}, // -X
		{2, 0}, // +Y
		{3, 0}, // -Y
		{4, 0}, // +Z
		{5, 0}  // -Z
	},
	
	// Blender
	{
		{0, 0}, // +X
		{2, 0}, // -X
		{1, 1}, // +Y
		{0, 1}, // -Y
		{1, 0}, // +Z
		{2, 1}  // -Z
	}
};

static const bool CUBEMAP_LAYOUT_FLIPS[5][6][2] = 
{
	// Vertical cross
	{
		{true, true}, // +X
		{true, true}, // -X
		{true, true}, // +Y
		{true, true}, // -Y
		{true, true}, // +Z
		{false, false}, // -Z
	},
	
	// Horizontal cross
	{
		{true, true}, // +X
		{true, true}, // -X
		{true, true}, // +Y
		{true, true}, // -Y
		{true, true}, // +Z
		{true, true}, // -Z
	},
	
	// Vertical strip
	{
		{true, true}, // +X
		{true, true}, // -X
		{true, true}, // +Y
		{true, true}, // -Y
		{true, true}, // +Z
		{true, true}, // -Z
	},
	
	// Horizontal strip
	{
		{true, true}, // +X
		{true, true}, // -X
		{true, true}, // +Y
		{true, true}, // -Y
		{true, true}, // +Z
		{true, true}, // -Z
	},
	
	// Blender
	{
		{true, true}, // +X
		{true, true}, // -X
		{true, true}, // +Y
		{true, true}, // -Y
		{true, true}, // +Z
		{true, true}, // -Z
	}
};

static void loadCubemapMipmap(TextureCube* texture, std::size_t level, Image* image)
{
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
		glTexParameteriv(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_SWIZZLE_RGBA, swizzleMask);
	}
	else if (image->getChannels() == 2)
	{
		// Grey, alpha
		internalFormat = (image->isHDR()) ? GL_RG32F : GL_RG8;
		format = GL_RG;
		
		GLint swizzleMask[] = {GL_RED, GL_RED, GL_RED, GL_GREEN};
		glTexParameteriv(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_SWIZZLE_RGBA, swizzleMask);
	}
	else if (image->getChannels() == 3)
	{
		// Red, green, blue
		internalFormat = (image->isHDR()) ? GL_RGB32F : GL_RGB8;
		format = GL_RGB;
		
		GLint swizzleMask[] = {GL_RED, GL_GREEN, GL_BLUE, GL_ONE};
		glTexParameteriv(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_SWIZZLE_RGBA, swizzleMask);
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
		stream << std::string("Mipmap cannot be loaded from an image with an unsupported number of color channels (") << image->getChannels() << std::string(").");
		throw std::runtime_error(stream.str().c_str());
	}

	// Determine cubemap layout
	int layout = -1;
	for (std::size_t i = 0; i < 5; ++i)
	{
		// Compare aspect ratio of image to known cubemap layouts
		float aspectRatio = static_cast<float>(CUBEMAP_LAYOUT_DIMENSIONS[i][0]) / static_cast<float>(CUBEMAP_LAYOUT_DIMENSIONS[i][1]);
		if (static_cast<unsigned int>((static_cast<float>(image->getHeight()) * aspectRatio)) == image->getWidth())
		{
			layout = i;
			break;
		}
	}
		
	// Check that a layout was determined
	if (layout == -1)
	{
		throw std::runtime_error("Unsupported cubemap layout.");
	}

	// Calculate cubemap face size
	int faceSize = image->getWidth() / CUBEMAP_LAYOUT_DIMENSIONS[layout][0];
	if (!level)
	{
		texture->setFaceSize(faceSize);
	}

	// Allocate cubemap face pixels
	void* facePixels = nullptr;
	if (image->isHDR())
	{
		facePixels = new float[faceSize * faceSize * image->getChannels()];
	}
	else
	{
		facePixels = new unsigned char[faceSize * faceSize * image->getChannels()];
	}

	// Upload mipmap data to OpenGL
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	for (std::size_t faceIndex = 0; faceIndex < 6; ++faceIndex)
	{
		int offsetX = faceSize * CUBEMAP_LAYOUT_OFFSETS[layout][faceIndex][0];
		int offsetY = faceSize * ((CUBEMAP_LAYOUT_DIMENSIONS[layout][1] - 1) - CUBEMAP_LAYOUT_OFFSETS[layout][faceIndex][1]);
		bool flipX = CUBEMAP_LAYOUT_FLIPS[layout][faceIndex][0];
		bool flipY = CUBEMAP_LAYOUT_FLIPS[layout][faceIndex][1];
		
		for (int y = 0; y < faceSize; ++y)
		{
			for (int x = 0; x < faceSize; ++x)
			{
				std::size_t facePixelIndex;
				
				int faceX = (flipX) ? (faceSize - 1 - x) : x;
				int faceY = (flipY) ? (faceSize - 1 - y) : y;
				facePixelIndex = (faceY * faceSize + faceX) * image->getChannels();
				
				std::size_t cubemapPixelIndex = ((offsetY + y) * image->getWidth() + (offsetX + x)) * image->getChannels();
				
				if (image->isHDR())
				{
					for (int c = 0; c < image->getChannels(); ++c)
					{
						reinterpret_cast<float*>(facePixels)[facePixelIndex + c] = static_cast<float*>(image->getPixels())[cubemapPixelIndex + c];
					}
				}
				else
				{
					for (int c = 0; c < image->getChannels(); ++c)
					{
						reinterpret_cast<unsigned char*>(facePixels)[facePixelIndex + c] = static_cast<unsigned char*>(image->getPixels())[cubemapPixelIndex + c];
					}
				}
			}
		}
		
		// Upload cubemap face pixels to OpenGL
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + faceIndex, level, internalFormat, faceSize, faceSize, 0, format, type, facePixels);
	}

	// Free cubemap face pixels
	if (image->isHDR())
	{
		delete[] reinterpret_cast<float*>(facePixels);
	}
	else
	{
		delete[] reinterpret_cast<unsigned char*>(facePixels);
	}
}

template <>
TextureCube* ResourceLoader<TextureCube>::load(ResourceManager* resourceManager, std::istream* is)
{
	// Load list of mipmap filenames
	TextFile* mipmapFilenames = ResourceLoader<TextFile>::load(resourceManager, is);

	// Allocate and initialize mipmap array
	std::size_t mipmapCount = mipmapFilenames->size();
	Image** mipmaps = new Image*[mipmapCount];
	for (std::size_t i = 0; i < mipmapCount; ++i)
	{
		mipmaps[i] = nullptr;
	}

	// Generate OpenGL texture ID
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	
	// Set wrapping and filtering parameters
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Create texture cube
	TextureCube* texture = new TextureCube();
	texture->setTextureID(textureID);

	// Load mipmaps
	std::size_t loadedMipmapCount = 0;
	try
	{
		for (std::size_t i = 0; i < mipmapCount; ++i)
		{
			// Load mipmap image data
			mipmaps[i] = resourceManager->load<Image>((*mipmapFilenames)[i]);

			// Load mipmap from image data
			loadCubemapMipmap(texture, i, mipmaps[i]);

			// Unload mipmap image data
			resourceManager->unload((*mipmapFilenames)[i]);
			mipmaps[i] = nullptr;

			++loadedMipmapCount;
		}
	}
	catch (const std::exception& e)
	{
		std::string error = std::string("ResourceLoader<TextureCube>::load(): Failed to load mipmap \"") + (*mipmapFilenames)[loadedMipmapCount] + std::string("\": \"") + e.what() + std::string("\"");

		// Free TextureCube
		delete texture;

		// Delete OpenGL texture
		glDeleteTextures(1, &textureID);

		// Free loaded mipmap images
		for (std::size_t i = 0; i < mipmapCount; ++i)
		{
			if (mipmaps[i] != nullptr)
			{
				resourceManager->unload((*mipmapFilenames)[i]);
			}
		}
		delete[] mipmaps;

		// Free list of mipmap filenames
		delete mipmapFilenames;

		throw std::runtime_error(error.c_str());
	}

	// Free mipmap array
	delete[] mipmaps;

	// Free list of mipmap filenames
	delete mipmapFilenames;

	return texture;
}


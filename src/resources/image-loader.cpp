/*
 * Copyright (C) 2021  Christopher J. Howard
 *
 * This file is part of Antkeeper source code.
 *
 * Antkeeper source code is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Antkeeper source code is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Antkeeper source code.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "resource-loader.hpp"
#include "stb/stb_image.h"
#include "resources/image.hpp"
#include <cstring>
#include <stdexcept>
#include <physfs.h>
#include <tinyexr.h>

template <>
image* resource_loader<image>::load(resource_manager* resource_manager, PHYSFS_File* file, const std::filesystem::path& path)
{
	unsigned char* buffer;
	int size;
	::image* image = nullptr;

	// Read input stream into buffer
	size = static_cast<int>(PHYSFS_fileLength(file));
	buffer = new unsigned char[size];
	PHYSFS_readBytes(file, buffer, size);
	
	// Select loader according to file extension
	if (path.extension() == ".exr")
	{
		// Load OpenEXR with TinyEXR
		
		float* pixels = nullptr;
		int width = 0;
		int height = 0;
		const char* error = nullptr;
		int status = LoadEXRFromMemory(&pixels, &width, &height, buffer, size, &error);
		
		if (status != TINYEXR_SUCCESS)
		{
			delete[] buffer;
			std::string error_string(error);
			FreeEXRErrorMessage(error);
			throw std::runtime_error("TinyEXR error (" + std::to_string(status) + "): " + error_string);
		}
		
		// Create image
		std::size_t component_size = sizeof(float);
		image = new ::image();
		image->format(component_size, 4);
		image->resize(static_cast<unsigned int>(width), static_cast<unsigned int>(height));
		std::memcpy(image->get_pixels(), pixels, image->get_size());
		
		// Free loaded pixels
		free(pixels);
	}
	else
	{
		// Load all other formats with stb_image
		
		// Determine if image is in an HDR format
		bool hdr = (stbi_is_hdr_from_memory(buffer, size) != 0);
		
		// Set vertical flip on load in order to upload pixels correctly to OpenGL
		stbi_set_flip_vertically_on_load(true);
		
		// Load image data
		void* pixels = nullptr;
		int width = 0;
		int height = 0;
		int channels = 0;
		if (hdr)
		{
			pixels = stbi_loadf_from_memory(buffer, size, &width, &height, &channels, 0);
		}
		else
		{
			pixels = stbi_load_from_memory(buffer, size, &width, &height, &channels, 0);
		}
		
		// Free file buffer
		delete[] buffer;
		
		// Check if image was loaded
		if (!pixels)
		{
			throw std::runtime_error("STBI failed to load image from memory.");
		}
		
		// Create image
		std::size_t component_size = (hdr) ? sizeof(float) : sizeof(unsigned char);
		image = new ::image();
		image->format(component_size, channels);
		image->resize(static_cast<unsigned int>(width), static_cast<unsigned int>(height));
		std::memcpy(image->get_pixels(), pixels, image->get_size());
		
		// Free loaded image data
		stbi_image_free(pixels);
	}

	return image;
}


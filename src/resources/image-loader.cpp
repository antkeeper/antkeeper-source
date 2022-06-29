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
		int status = TINYEXR_SUCCESS;
		const char* error = nullptr;
		
		// Read EXR version
		EXRVersion exr_version;
		status = ParseEXRVersionFromMemory(&exr_version, buffer, size);
		if (status != TINYEXR_SUCCESS)
		{
			delete[] buffer;
			throw std::runtime_error("TinyEXR parse version error (" + std::to_string(status) + "): invalid EXR file");
		}
		
		// Check if image is multipart
		if (exr_version.multipart)
		{
			throw std::runtime_error("OpenEXR multipart images not supported");
		}
		
		// Read EXR header
		EXRHeader exr_header;
		InitEXRHeader(&exr_header);
		status = ParseEXRHeaderFromMemory(&exr_header, &exr_version, buffer, size, &error);
		if (status != TINYEXR_SUCCESS)
		{
			std::string error_string(error);
			FreeEXRErrorMessage(error);
			delete[] buffer;
			throw std::runtime_error("TinyEXR parse header error (" + std::to_string(status) + "): " + error_string);
		}
		
		// Check if image is tiled
		if (exr_header.tiled)
		{
			FreeEXRHeader(&exr_header);
			delete[] buffer;
			throw std::runtime_error("OpenEXR tiled images not supported");
		}
		
		// Read half channels as float
		for (int i = 0; i < exr_header.num_channels; ++i)
		{
			if (exr_header.pixel_types[i] == TINYEXR_PIXELTYPE_HALF)
			{
				exr_header.requested_pixel_types[i] = TINYEXR_PIXELTYPE_FLOAT;
			}
		}
		
		// Read EXR data
		EXRImage exr_image;
		InitEXRImage(&exr_image);
		status = LoadEXRImageFromMemory(&exr_image, &exr_header, buffer, size, &error);
		if (status != TINYEXR_SUCCESS)
		{
			std::string error_string(error);
			FreeEXRErrorMessage(error);
			FreeEXRHeader(&exr_header);
			delete[] buffer;
			throw std::runtime_error("TinyEXR load error (" + std::to_string(status) + "): " + error_string);
		}
		
		// Free file buffer
		delete[] buffer;
		
		// Create image
		image = new ::image();
		image->format(sizeof(float), exr_image.num_channels);
		image->resize(static_cast<unsigned int>(exr_image.width), static_cast<unsigned int>(exr_image.height));
		
		// Fill image pixels
		float* component = static_cast<float*>(image->get_pixels());
		for (int y = exr_image.height - 1; y >= 0; --y)
		{
			int row_offset = y * exr_image.width;
			
			for (int x = 0; x < exr_image.width; ++x)
			{
				int pixel_index = row_offset + x;
				
				for (int c = exr_image.num_channels - 1; c >= 0; --c)
				{
					*(component++) = reinterpret_cast<float**>(exr_image.images)[c][pixel_index];
				}
			}
		}
		
		// Free EXR data
		FreeEXRImage(&exr_image);
		FreeEXRHeader(&exr_header);
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


/*
 * Copyright (C) 2020  Christopher J. Howard
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

template <>
image* resource_loader<image>::load(resource_manager* resource_manager, std::istream* is)
{
	unsigned char* buffer;
	int size;
	int width;
	int height;
	int channels;
	bool hdr;
	void* pixels;

	// Read input stream into buffer
	is->seekg(0, is->end);
	size = static_cast<int>(is->tellg());
	buffer = new unsigned char[size];
	is->seekg(0, is->beg);
	is->read(reinterpret_cast<char*>(&buffer[0]), size);

	// Determine if image is in an HDR format
	hdr = (stbi_is_hdr_from_memory(buffer, size) != 0);
	
	// Set vertical flip on load in order to upload pixels correctly to OpenGL
	stbi_set_flip_vertically_on_load(true);
	
	// Load image data
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

	::image* image = new ::image();
	image->format(static_cast<unsigned int>(channels), hdr);
	image->resize(static_cast<unsigned int>(width), static_cast<unsigned int>(height));
	std::memcpy(image->get_pixels(), pixels, width * height * channels * ((hdr) ? sizeof(float) : sizeof(unsigned char)));
	
	// Free loaded image data
	stbi_image_free(pixels);

	return image;
}


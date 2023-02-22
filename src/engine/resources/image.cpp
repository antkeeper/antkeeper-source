/*
 * Copyright (C) 2023  Christopher J. Howard
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

#include <engine/resources/image.hpp>
#include <cstring>
#include <stdexcept>

image::image(const image& source)
{
	*this = source;
}

image::image():
	width(0),
	height(0),
	component_size(0),
	channel_count(0),
	pixel_size(0),
	pixels(nullptr),
	size(0)
{}

image::~image()
{
	free_pixels();
}

image& image::operator=(const image& source)
{
	format(source.component_size, source.channel_count);
	resize(source.width, source.height);
	std::memcpy(pixels, source.pixels, size);
	
	return *this;
}

bool image::compatible(const image& other) const
{
	return (other.component_size == component_size && other.channel_count == channel_count);
}

void image::copy(const image& source, unsigned int w, unsigned int h, unsigned int from_x, int unsigned from_y, unsigned int to_x, unsigned int to_y)
{
	if (!compatible(source))
	{
		throw std::runtime_error("Cannot copy image with mismatched format");
	}
	
	const unsigned char* from_pixels = static_cast<const unsigned char*>(source.pixels);
	unsigned char* to_pixels = static_cast<unsigned char*>(pixels);
	
	for (unsigned int i = 0; i < h; ++i)
	{
		// Calculate vertical pixel offset
		unsigned int from_i = from_y + i;
		unsigned int to_i = to_y + i;
		
		// Bounds check
		if (from_i >= source.height || to_i >= height)
			break;
		
		for (unsigned int j = 0; j < w; ++j)
		{
			// Calculate horizontal pixel offsets
			unsigned int from_j = from_x + j;
			unsigned int to_j = to_x + j;
			
			// Bounds check
			if (from_j >= source.width || to_j >= width)
				continue;
			
			// Calculate pixel data offset (in bytes)
			std::size_t from_offset = (from_i * source.width + from_j) * pixel_size;
			std::size_t to_offset = (to_i * width + to_j) * pixel_size;
			
			// Copy single pixel
			std::memcpy(to_pixels + to_offset, from_pixels + from_offset, pixel_size);
		}
	}
}

void image::format(std::size_t component_size, std::size_t channel_count)
{
	if (this->component_size == component_size && this->channel_count == channel_count)
		return;
	
	free_pixels();
	this->component_size = component_size;
	this->channel_count = channel_count;
	pixel_size = component_size * channel_count;
	size = width * height * pixel_size;
	allocate_pixels();
}

void image::resize(unsigned int width, unsigned int height)
{
	if (this->width == width && this->height == height)
	{
		return;
	}

	free_pixels();
	this->width = width;
	this->height = height;
	size = width * height * pixel_size;
	allocate_pixels();
}

void image::allocate_pixels()
{
	if (size != 0)
	{
		pixels = new unsigned char[size];
	}
}

void image::free_pixels()
{
	if (pixels != nullptr)
	{
		delete[] reinterpret_cast<unsigned char*>(pixels);
		pixels = nullptr;
		size = 0;
	}
}

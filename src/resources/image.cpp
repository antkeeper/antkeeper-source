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

#include "image.hpp"

image::image():
	hdr(false),
	width(0),
	height(0),
	channels(4),
	pixels(nullptr)
{}

image::~image()
{
	free_pixels();
}

void image::format(unsigned int channels, bool hdr)
{
	if (this->channels == channels && this->hdr == hdr)
	{
		return;
	}

	free_pixels();
	this->channels = channels;
	this->hdr = hdr;
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
	allocate_pixels();
}

void image::allocate_pixels()
{
	if (hdr)
	{
		pixels = new float[width * height * channels];
	}
	else
	{
		pixels = new unsigned char[width * height * channels];
	}
}

void image::free_pixels()
{
	if (pixels != nullptr)
	{
		if (hdr)
		{
			delete[] reinterpret_cast<float*>(pixels);
		}
		else
		{
			delete[] reinterpret_cast<unsigned char*>(pixels);
		}

		pixels = nullptr;
	}
}


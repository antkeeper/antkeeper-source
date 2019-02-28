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

#include "image.hpp"

Image::Image():
	hdr(false),
	width(0),
	height(0),
	channels(4),
	pixels(nullptr)
{}

Image::~Image()
{
	freePixels();
}

void Image::format(unsigned int channels, bool hdr)
{
	if (this->channels == channels && this->hdr == hdr)
	{
		return;
	}

	freePixels();
	this->channels = channels;
	this->hdr = hdr;
	allocatePixels();
}

void Image::resize(unsigned int width, unsigned int height)
{
	if (this->width == width && this->height == height)
	{
		return;
	}

	freePixels();
	this->width = width;
	this->height = height;
	allocatePixels();
}

void Image::allocatePixels()
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

void Image::freePixels()
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


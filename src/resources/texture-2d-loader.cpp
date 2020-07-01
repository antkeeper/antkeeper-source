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

#include "resources/resource-loader.hpp"
#include "resources/image.hpp"
#include "rasterizer/pixel-type.hpp"
#include "rasterizer/pixel-format.hpp"
#include "rasterizer/texture-2d.hpp"
#include <sstream>

template <>
texture_2d* resource_loader<texture_2d>::load(resource_manager* resource_manager, std::istream* is)
{
	// Load image
	::image* image = resource_loader<::image>::load(resource_manager, is);

	// Determine pixel type
	pixel_type type = (image->is_hdr()) ? pixel_type::float_32 : pixel_type::uint_8;

	// Determine pixel format
	pixel_format format;
	if (image->get_channels() == 1)
	{
		format = pixel_format::r;
	}
	else if (image->get_channels() == 2)
	{
		format = pixel_format::rg;
	}
	else if (image->get_channels() == 3)
	{
		format = pixel_format::rgb;
	}
	else if (image->get_channels() == 4)
	{
		format = pixel_format::rgba;
	}
	else
	{
		std::stringstream stream;
		stream << std::string("Texture cannot be created from an image with an unsupported number of color channels (") << image->get_channels() << std::string(").");
		delete image;
		throw std::runtime_error(stream.str().c_str());
	}

	// Create texture
	texture_2d* texture = new texture_2d(image->get_width(), image->get_height(), type, format, image->get_pixels());

	// Free loaded image
	delete image;

	return texture;
}


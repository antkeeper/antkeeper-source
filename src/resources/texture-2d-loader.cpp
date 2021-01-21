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

#include "resources/resource-loader.hpp"
#include "resources/image.hpp"
#include "gl/pixel-type.hpp"
#include "gl/pixel-format.hpp"
#include "gl/texture-2d.hpp"
#include <sstream>

template <>
gl::texture_2d* resource_loader<gl::texture_2d>::load(resource_manager* resource_manager, PHYSFS_File* file)
{
	// Load image
	::image* image = resource_loader<::image>::load(resource_manager, file);

	// Determine pixel type
	gl::pixel_type type = (image->is_hdr()) ? gl::pixel_type::float_32 : gl::pixel_type::uint_8;

	// Determine pixel format
	gl::pixel_format format;
	if (image->get_channels() == 1)
	{
		format = gl::pixel_format::r;
	}
	else if (image->get_channels() == 2)
	{
		format = gl::pixel_format::rg;
	}
	else if (image->get_channels() == 3)
	{
		format = gl::pixel_format::rgb;
	}
	else if (image->get_channels() == 4)
	{
		format = gl::pixel_format::rgba;
	}
	else
	{
		std::stringstream stream;
		stream << std::string("Texture cannot be created from an image with an unsupported number of color channels (") << image->get_channels() << std::string(").");
		delete image;
		throw std::runtime_error(stream.str().c_str());
	}
	
	// Assume linear color space
	gl::color_space color_space = gl::color_space::linear;

	// Create texture
	gl::texture_2d* texture = new gl::texture_2d(image->get_width(), image->get_height(), type, format, color_space, image->get_pixels());

	// Free loaded image
	delete image;

	return texture;
}

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
#include "resources/resource-manager.hpp"
#include "resources/image.hpp"
#include "gl/pixel-type.hpp"
#include "gl/pixel-format.hpp"
#include "gl/color-space.hpp"
#include "gl/texture-1d.hpp"
#include "gl/texture-2d.hpp"
#include "gl/texture-wrapping.hpp"
#include "gl/texture-filter.hpp"
#include <sstream>
#include <stdexcept>
#include <nlohmann/json.hpp>
#include <physfs.h>

template <>
gl::texture_1d* resource_loader<gl::texture_1d>::load(resource_manager* resource_manager, PHYSFS_File* file, const std::filesystem::path& path)
{
	// Read file into buffer
	std::size_t size = static_cast<int>(PHYSFS_fileLength(file));
	std::string buffer;
	buffer.resize(size);
	PHYSFS_readBytes(file, &buffer[0], size);
	
	// Parse json from file buffer
	nlohmann::json json = nlohmann::json::parse(buffer);
	
	// Read image filename
	std::string image_filename;
	if (auto element = json.find("image"); element != json.end())
		image_filename = element.value().get<std::string>();
	
	// Read color space
	gl::color_space color_space = gl::color_space::linear;
	if (auto element = json.find("color_space"); element != json.end())
	{
		std::string value = element.value().get<std::string>();
		if (value == "linear")
			color_space = gl::color_space::linear;
		else if (value == "srgb")
			color_space = gl::color_space::srgb;
	}
	
	// Read extension mode
	gl::texture_wrapping wrapping = gl::texture_wrapping::repeat;
	if (auto element = json.find("extension"); element != json.end())
	{
		std::string value = element.value().get<std::string>();
		if (value == "clip")
			wrapping = gl::texture_wrapping::clip;
		else if (value == "extend")
			wrapping = gl::texture_wrapping::extend;
		else if (value == "repeat")
			wrapping = gl::texture_wrapping::repeat;
		else if (value == "mirrored_repeat")
			wrapping = gl::texture_wrapping::mirrored_repeat;
	}
	
	// Read interpolation mode
	gl::texture_min_filter min_filter = gl::texture_min_filter::linear_mipmap_linear;
	gl::texture_mag_filter mag_filter = gl::texture_mag_filter::linear;
	if (auto element = json.find("interpolation"); element != json.end())
	{
		std::string value = element.value().get<std::string>();
		if (value == "linear")
		{
			min_filter = gl::texture_min_filter::linear_mipmap_linear;
			mag_filter = gl::texture_mag_filter::linear;
		}
		else if (value == "closest")
		{
			min_filter = gl::texture_min_filter::nearest_mipmap_nearest;
			mag_filter = gl::texture_mag_filter::nearest;
		}
	}
	
	// Read max anisotropy
	float max_anisotropy = 0.0f;
	if (auto element = json.find("max_anisotropy"); element != json.end())
		max_anisotropy = element.value().get<float>();
	
	// Load image
	::image* image = resource_manager->load<::image>(image_filename);
	
	// Determine pixel type
	gl::pixel_type type = (image->get_component_size() == sizeof(float)) ? gl::pixel_type::float_32 : gl::pixel_type::uint_8;

	// Determine pixel format
	gl::pixel_format format;
	if (image->get_channel_count() == 1)
	{
		format = gl::pixel_format::r;
	}
	else if (image->get_channel_count() == 2)
	{
		format = gl::pixel_format::rg;
	}
	else if (image->get_channel_count() == 3)
	{
		format = gl::pixel_format::rgb;
	}
	else if (image->get_channel_count() == 4)
	{
		format = gl::pixel_format::rgba;
	}
	else
	{
		std::stringstream stream;
		stream << std::string("Texture cannot be created from an image with an unsupported number of channels (") << image->get_channel_count() << std::string(").");
		delete image;
		throw std::runtime_error(stream.str().c_str());
	}
	
	// Create texture
	gl::texture_1d* texture = new gl::texture_1d(image->get_width(), type, format, color_space, image->get_pixels());
	
	// Set wrapping and filtering
	texture->set_wrapping(wrapping);
	texture->set_filters(min_filter, mag_filter);
	texture->set_max_anisotropy(max_anisotropy);
	
	// Free loaded image
	resource_manager->unload(image_filename);

	return texture;
}

template <>
gl::texture_2d* resource_loader<gl::texture_2d>::load(resource_manager* resource_manager, PHYSFS_File* file, const std::filesystem::path& path)
{
	// Read file into buffer
	std::size_t size = static_cast<int>(PHYSFS_fileLength(file));
	std::string buffer;
	buffer.resize(size);
	PHYSFS_readBytes(file, &buffer[0], size);
	
	// Parse json from file buffer
	nlohmann::json json = nlohmann::json::parse(buffer);
	
	// Read image filename
	std::string image_filename;
	if (auto element = json.find("image"); element != json.end())
		image_filename = element.value().get<std::string>();
	
	// Read color space
	gl::color_space color_space = gl::color_space::linear;
	if (auto element = json.find("color_space"); element != json.end())
	{
		std::string value = element.value().get<std::string>();
		if (value == "linear")
			color_space = gl::color_space::linear;
		else if (value == "srgb")
			color_space = gl::color_space::srgb;
	}
	
	// Read extension mode
	gl::texture_wrapping wrapping = gl::texture_wrapping::repeat;
	if (auto element = json.find("extension"); element != json.end())
	{
		std::string value = element.value().get<std::string>();
		if (value == "clip")
			wrapping = gl::texture_wrapping::clip;
		else if (value == "extend")
			wrapping = gl::texture_wrapping::extend;
		else if (value == "repeat")
			wrapping = gl::texture_wrapping::repeat;
		else if (value == "mirrored_repeat")
			wrapping = gl::texture_wrapping::mirrored_repeat;
	}
	
	// Read interpolation mode
	gl::texture_min_filter min_filter = gl::texture_min_filter::linear_mipmap_linear;
	gl::texture_mag_filter mag_filter = gl::texture_mag_filter::linear;
	if (auto element = json.find("interpolation"); element != json.end())
	{
		std::string value = element.value().get<std::string>();
		if (value == "linear")
		{
			min_filter = gl::texture_min_filter::linear_mipmap_linear;
			mag_filter = gl::texture_mag_filter::linear;
		}
		else if (value == "closest")
		{
			min_filter = gl::texture_min_filter::nearest_mipmap_nearest;
			mag_filter = gl::texture_mag_filter::nearest;
		}
	}
	
	// Read max anisotropy
	float max_anisotropy = 0.0f;
	if (auto element = json.find("max_anisotropy"); element != json.end())
		max_anisotropy = element.value().get<float>();
	
	// Load image
	::image* image = resource_manager->load<::image>(image_filename);
	
	// Determine pixel type
	gl::pixel_type type = (image->get_component_size() == sizeof(float)) ? gl::pixel_type::float_32 : gl::pixel_type::uint_8;

	// Determine pixel format
	gl::pixel_format format;
	if (image->get_channel_count() == 1)
	{
		format = gl::pixel_format::r;
	}
	else if (image->get_channel_count() == 2)
	{
		format = gl::pixel_format::rg;
	}
	else if (image->get_channel_count() == 3)
	{
		format = gl::pixel_format::rgb;
	}
	else if (image->get_channel_count() == 4)
	{
		format = gl::pixel_format::rgba;
	}
	else
	{
		std::stringstream stream;
		stream << std::string("Texture cannot be created from an image with an unsupported number of channels (") << image->get_channel_count() << std::string(").");
		delete image;
		throw std::runtime_error(stream.str().c_str());
	}

	// Create texture
	gl::texture_2d* texture = new gl::texture_2d(image->get_width(), image->get_height(), type, format, color_space, image->get_pixels());
	
	// Set wrapping and filtering
	texture->set_wrapping(wrapping, wrapping);
	texture->set_filters(min_filter, mag_filter);
	texture->set_max_anisotropy(max_anisotropy);
	
	// Free loaded image
	resource_manager->unload(image_filename);

	return texture;
}



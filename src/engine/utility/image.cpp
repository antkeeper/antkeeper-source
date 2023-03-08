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

#include <engine/utility/image.hpp>
#include <cstring>
#include <engine/resources/resource-loader.hpp>
#include <engine/resources/deserialize-error.hpp>
#include <engine/resources/deserializer.hpp>
#include <stb/stb_image.h>
#include <stdexcept>
#include <tinyexr.h>

bool image::compatible(const image& other) const noexcept
{
	return (other.m_component_size == m_component_size && other.m_channel_count == m_channel_count);
}

void image::copy
(
	const image& source,
	std::uint32_t w,
	std::uint32_t h,
	std::uint32_t from_x,
	std::uint32_t from_y,
	std::uint32_t to_x,
	std::uint32_t to_y
)
{
	if (!compatible(source))
	{
		throw std::runtime_error("Cannot copy image with mismatched format");
	}
	
	const std::byte* from_pixels = source.pixels.data();
	std::byte* to_pixels = pixels.data();
	
	for (std::uint32_t i = 0; i < h; ++i)
	{
		// Calculate vertical pixel offset
		std::uint32_t from_i = from_y + i;
		std::uint32_t to_i = to_y + i;
		
		// Bounds check
		if (from_i >= source.m_height || to_i >= m_height)
		{
			break;
		}
		
		for (std::uint32_t j = 0; j < w; ++j)
		{
			// Calculate horizontal pixel offsets
			std::uint32_t from_j = from_x + j;
			std::uint32_t to_j = to_x + j;
			
			// Bounds check
			if (from_j >= source.m_width || to_j >= m_width)
			{
				continue;
			}
			
			// Calculate pixel data offset (in bytes)
			std::size_t from_offset = (from_i * source.m_width + from_j) * m_pixel_size;
			std::size_t to_offset = (to_i * m_width + to_j) * m_pixel_size;
			
			// Copy single pixel
			std::memcpy(to_pixels + to_offset, from_pixels + from_offset, m_pixel_size);
		}
	}
}

void image::format(std::size_t component_size, std::uint8_t channel_count)
{
	if (m_component_size != component_size || m_channel_count != channel_count)
	{
		m_component_size = component_size;
		m_channel_count = channel_count;
		m_pixel_size = m_component_size * m_channel_count;
		pixels.resize(m_width * m_height * m_pixel_size);
	}
}

void image::resize(std::uint32_t width, std::uint32_t height)
{
	if (m_width != width || m_height == height)
	{
		m_width = width;
		m_height = height;
		pixels.resize(m_width * m_height * m_pixel_size);
	}
}

static void deserialize_tinyexr(image& image, deserialize_context& ctx)
{
	const char* error = nullptr;
	
	// Read data into file buffer
	std::vector<unsigned char> file_buffer(ctx.size());
	ctx.read8(reinterpret_cast<std::byte*>(file_buffer.data()), file_buffer.size());
	
	// Read EXR version
	EXRVersion exr_version;
	if (int status = ParseEXRVersionFromMemory(&exr_version, file_buffer.data(), file_buffer.size()); status != TINYEXR_SUCCESS)
	{
		throw deserialize_error(std::format("TinyEXR version parse error {}", status));
	}
	
	// Check if image is multipart
	if (exr_version.multipart)
	{
		throw deserialize_error("OpenEXR multipart images not supported");
	}
	
	// Init and read EXR header data
	EXRHeader exr_header;
	InitEXRHeader(&exr_header);
	if (int status = ParseEXRHeaderFromMemory(&exr_header, &exr_version, file_buffer.data(), file_buffer.size(), &error); status != TINYEXR_SUCCESS)
	{
		const std::string error_message(error);
		FreeEXRErrorMessage(error);
		throw deserialize_error(error_message);
	}
	
	// Check if image is tiled
	if (exr_header.tiled)
	{
		FreeEXRHeader(&exr_header);
		throw deserialize_error("OpenEXR tiled images not supported");
	}
	
	// Read half channels as float
	for (int i = 0; i < exr_header.num_channels; ++i)
	{
		if (exr_header.pixel_types[i] == TINYEXR_PIXELTYPE_HALF)
		{
			exr_header.requested_pixel_types[i] = TINYEXR_PIXELTYPE_FLOAT;
		}
	}
	
	// Init and read EXR image data
	EXRImage exr_image;
	InitEXRImage(&exr_image);
	if (int status = LoadEXRImageFromMemory(&exr_image, &exr_header, file_buffer.data(), file_buffer.size(), &error); status != TINYEXR_SUCCESS)
	{
		const std::string error_message(error);
		FreeEXRErrorMessage(error);
		FreeEXRHeader(&exr_header);
		throw deserialize_error(error_message);
	}
	
	// Free file buffer
	file_buffer.clear();
	
	// Format and resize image
	image.format(sizeof(float), static_cast<std::uint8_t>(exr_image.num_channels));
	image.resize(static_cast<std::uint32_t>(exr_image.width), static_cast<std::uint32_t>(exr_image.height));
	
	// Fill image pixels
	float* component = reinterpret_cast<float*>(image.data());
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
	
	// Free EXR image and header data
	FreeEXRImage(&exr_image);
	FreeEXRHeader(&exr_header);
}

static int stb_io_read(void* user, char* data, int size)
{
	deserialize_context& ctx = *static_cast<deserialize_context*>(user);
	return static_cast<int>(ctx.read8(reinterpret_cast<std::byte*>(data), static_cast<std::size_t>(size)));
}

static void stb_io_skip(void* user, int n)
{
	deserialize_context& ctx = *static_cast<deserialize_context*>(user);
	ctx.seek(ctx.tell() + n);
}

static int stb_io_eof(void* user)
{
	deserialize_context& ctx = *static_cast<deserialize_context*>(user);
	return static_cast<int>(ctx.eof());
}

static void deserialize_stb_image(image& image, deserialize_context& ctx)
{
	// Set vertical flip on load in order to upload pixels correctly to OpenGL
	stbi_set_flip_vertically_on_load(true);
	
	// Setup IO callbacks
	const stbi_io_callbacks io_callbacks
	{
		&stb_io_read,
		&stb_io_skip,
		&stb_io_eof
	};
	
	// Load image data
	int width = 0;
	int height = 0;
	int channels = 0;
	stbi_uc* pixels = stbi_load_from_callbacks(&io_callbacks, &ctx, &width, &height, &channels, 0);
	if (!pixels)
	{
		throw deserialize_error(stbi_failure_reason());
	}
	
	// Create image
	image.format(sizeof(stbi_uc), static_cast<std::uint8_t>(channels));
	image.resize(static_cast<std::uint32_t>(width), static_cast<std::uint32_t>(height));
	std::memcpy(image.data(), pixels, image.size());
	
	// Free loaded image data
	stbi_image_free(pixels);
}

/**
 * Deserializes an image.
 *
 * @param[out] image Image to deserialize.
 * @param[in,out] ctx Deserialize context.
 *
 * @throw deserialize_error Read error.
 */
template <>
void deserializer<image>::deserialize(image& image, deserialize_context& ctx)
{
	// Select loader according to file extension
	if (ctx.path().extension() == ".exr")
	{
		// Deserialize EXR images with TinyEXR
		deserialize_tinyexr(image, ctx);
	}
	else
	{
		// Deserialize other image formats with stb_image
		deserialize_stb_image(image, ctx);
	}
}

template <>
std::unique_ptr<image> resource_loader<image>::load(::resource_manager& resource_manager, deserialize_context& ctx)
{
	auto resource = std::make_unique<image>();
	
	deserializer<image>().deserialize(*resource, ctx);
	
	return resource;
}

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
#include <engine/debug/log.hpp>
#include <stb/stb_image.h>
#include <stdexcept>
#include <tinyexr.h>

bool image::compatible(const image& other) const noexcept
{
	return (other.m_channels == m_channels && other.m_bit_depth == m_bit_depth);
}

void image::copy
(
	const image& source,
	const math::uvec2& dimensions,
	const math::uvec2& from,
	const math::uvec2& to
)
{
	if (!compatible(source))
	{
		throw std::runtime_error("Cannot copy image with mismatched format");
	}
	
	for (auto i = 0u; i < dimensions.y(); ++i)
	{
		// Calculate vertical pixel offset
		const auto from_i = from.y() + i;
		const auto to_i = to.y() + i;
		
		// Bounds check
		if (from_i >= source.m_size.y() || to_i >= m_size.y())
		{
			break;
		}
		
		for (auto j = 0u; j < dimensions.x(); ++j)
		{
			// Calculate horizontal pixel offsets
			const auto from_j = from.x() + j;
			const auto to_j = to.x() + j;
			
			// Bounds check
			if (from_j >= source.m_size.x() || to_j >= m_size.x())
			{
				continue;
			}
			
			// Calculate pixel data offset (in bytes)
			const auto from_offset = (static_cast<std::size_t>(from_i) * source.m_size.x() + from_j) * m_pixel_stride;
			const auto to_offset = (static_cast<std::size_t>(to_i) * m_size.x() + to_j) * m_pixel_stride;
			
			// Copy single pixel
			std::memcpy(data() + to_offset, source.data() + from_offset, m_pixel_stride);
		}
	}
}

void image::format(unsigned int channels, unsigned int bit_depth)
{
	if (bit_depth % 8 != 0)
	{
		throw std::runtime_error("Image bit depth must be byte-aligned");
	}
	
	if (m_channels != channels || m_bit_depth != bit_depth)
	{
		m_channels = channels;
		m_bit_depth = bit_depth;
		m_pixel_stride = m_channels * (m_bit_depth >> 3);
		m_sample_scale = static_cast<float>(1.0 / (std::exp2(m_bit_depth) - 1.0));
		m_data.resize(static_cast<std::size_t>(m_size.x()) * m_size.y() * m_size.z() * m_pixel_stride);
	}
}

void image::resize(const math::uvec3& size)
{
	if (m_size.x() != size.x() || m_size.y() != size.y() || m_size.z() != size.z())
	{
		m_size = size;
		m_data.resize(static_cast<std::size_t>(m_size.x()) * m_size.y() * m_size.z() * m_pixel_stride);
	}
}

math::fvec4 image::sample(std::size_t index) const
{
	math::fvec4 color{0, 0, 0, 1};
	
	const auto pixel_data = data() + index * m_pixel_stride;
	for (auto i = 0u; i < std::min(4u, m_channels); ++i)
	{
		std::uint32_t value = 0u;
		std::memcpy(&value, pixel_data + (m_bit_depth >> 3) * i, m_bit_depth >> 3);
		color[i] = static_cast<float>(value) * m_sample_scale;
	}
	
	return color;
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
	image.format(exr_image.num_channels, sizeof(float) * 8);
	image.resize({static_cast<unsigned int>(exr_image.width), static_cast<unsigned int>(exr_image.height), 1u});
	
	// Fill image pixels
	std::byte* component = image.data();
	for (int y = exr_image.height - 1; y >= 0; --y)
	{
		int row_offset = y * exr_image.width;
		
		for (int x = 0; x < exr_image.width; ++x)
		{
			int pixel_index = row_offset + x;
			
			for (int c = exr_image.num_channels - 1; c >= 0; --c)
			{
				std::memcpy(component, exr_image.images[c] + pixel_index * sizeof(float), sizeof(float));
				component += sizeof(float);
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
	
	int width = 0;
	int height = 0;
	int channels = 0;
	
	if (stbi_is_16_bit_from_callbacks(&io_callbacks, &ctx))
	{
		// Load 16-bit image
		ctx.seek(0);
		stbi_us* pixels = stbi_load_16_from_callbacks(&io_callbacks, &ctx, &width, &height, &channels, 0);
		if (!pixels)
		{
			throw deserialize_error(stbi_failure_reason());
		}
		
		// Format image and resize image, then copy pixel data
		image.format(static_cast<unsigned int>(channels), 16u);
		image.resize({static_cast<unsigned int>(width), static_cast<unsigned int>(height), 1u});
		std::memcpy(image.data(), pixels, image.size_bytes());
		
		// Free loaded image data
		stbi_image_free(pixels);
	}
	else
	{
		// Load 8-bit image
		ctx.seek(0);
		stbi_uc* pixels = stbi_load_from_callbacks(&io_callbacks, &ctx, &width, &height, &channels, 0);
		if (!pixels)
		{
			throw deserialize_error(stbi_failure_reason());
		}
		
		// Format image and resize image, then copy pixel data
		image.format(static_cast<unsigned int>(channels), 8u);
		image.resize({static_cast<unsigned int>(width), static_cast<unsigned int>(height), 1u});
		std::memcpy(image.data(), pixels, image.size_bytes());
		
		// Free loaded image data
		stbi_image_free(pixels);
	}
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

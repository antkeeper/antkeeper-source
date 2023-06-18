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

#include <engine/gl/texture.hpp>
#include <engine/gl/texture-1d.hpp>
#include <engine/gl/texture-2d.hpp>
#include <engine/gl/texture-3d.hpp>
#include <engine/gl/texture-cube.hpp>
#include <algorithm>
#include <engine/gl/color-space.hpp>
#include <engine/gl/pixel-format.hpp>
#include <engine/gl/pixel-type.hpp>
#include <engine/gl/texture-filter.hpp>
#include <engine/gl/texture-wrapping.hpp>
#include <engine/resources/deserialize-error.hpp>
#include <engine/resources/deserializer.hpp>
#include <engine/utility/image.hpp>
#include <engine/resources/resource-loader.hpp>
#include <engine/resources/resource-manager.hpp>
#include <engine/utility/json.hpp>
#include <glad/glad.h>

namespace gl {

static constexpr GLenum pixel_format_lut[] =
{
	GL_DEPTH_COMPONENT,
	GL_DEPTH_STENCIL,
	GL_RED,
	GL_RG,
	GL_RGB,
	GL_BGR,
	GL_RGBA,
	GL_BGRA
};

static constexpr GLenum pixel_type_lut[] =
{
	GL_BYTE,
	GL_UNSIGNED_BYTE,
	GL_SHORT,
	GL_UNSIGNED_SHORT,
	GL_INT,
	GL_UNSIGNED_INT,
	GL_HALF_FLOAT,
	GL_FLOAT
};

static constexpr GLenum linear_internal_format_lut[][8] =
{
	{GL_NONE, GL_NONE, GL_DEPTH_COMPONENT16, GL_DEPTH_COMPONENT16, GL_DEPTH_COMPONENT32, GL_DEPTH_COMPONENT32, GL_NONE, GL_DEPTH_COMPONENT32F},
	
	// Note: GL_DEPTH32F_STENCIL8 is actually a 64-bit format, 32 depth bits, 8 stencil bits, and 24 alignment bits.
	{GL_NONE, GL_NONE, GL_NONE, GL_NONE, GL_DEPTH24_STENCIL8, GL_DEPTH24_STENCIL8, GL_NONE, GL_DEPTH32F_STENCIL8},
	
	{GL_R8, GL_R8, GL_R16, GL_R16, GL_R32F, GL_R32F, GL_R16F, GL_R32F},
	{GL_RG8, GL_RG8, GL_RG16, GL_RG16, GL_RG32F, GL_RG32F, GL_RG16F, GL_RG32F},
	{GL_RGB8, GL_RGB8, GL_RGB16, GL_RGB16, GL_RGB32F, GL_RGB32F, GL_RGB16F, GL_RGB32F},
	{GL_RGB8, GL_RGB8, GL_RGB16, GL_RGB16, GL_RGB32F, GL_RGB32F, GL_RGB16F, GL_RGB32F},
	{GL_RGBA8, GL_RGBA8, GL_RGBA16, GL_RGBA16, GL_RGBA32F, GL_RGBA32F, GL_RGBA16F, GL_RGBA32F},
	{GL_RGBA8, GL_RGBA8, GL_RGBA16, GL_RGBA16, GL_RGBA32F, GL_RGBA32F, GL_RGBA16F, GL_RGBA32F}
};

static constexpr GLenum srgb_internal_format_lut[][8] =
{
	{GL_NONE, GL_NONE, GL_DEPTH_COMPONENT16, GL_DEPTH_COMPONENT16, GL_DEPTH_COMPONENT32, GL_DEPTH_COMPONENT32, GL_NONE, GL_DEPTH_COMPONENT32F},
	{GL_NONE, GL_NONE, GL_NONE, GL_NONE, GL_DEPTH24_STENCIL8, GL_DEPTH24_STENCIL8, GL_NONE, GL_DEPTH32F_STENCIL8},
	{GL_SRGB8, GL_SRGB8, GL_R16, GL_R16, GL_R32F, GL_R32F, GL_R16F, GL_R32F},
	{GL_SRGB8, GL_SRGB8, GL_RG16, GL_RG16, GL_RG32F, GL_RG32F, GL_RG16F, GL_RG32F},
	{GL_SRGB8, GL_SRGB8, GL_RGB16, GL_RGB16, GL_RGB32F, GL_RGB32F, GL_RGB16F, GL_RGB32F},
	{GL_SRGB8, GL_SRGB8, GL_RGB16, GL_RGB16, GL_RGB32F, GL_RGB32F, GL_RGB16F, GL_RGB32F},
	{GL_SRGB8_ALPHA8, GL_SRGB8_ALPHA8, GL_RGBA16, GL_RGBA16, GL_RGBA32F, GL_RGBA32F, GL_RGBA16F, GL_RGBA32F},
	{GL_SRGB8_ALPHA8, GL_SRGB8_ALPHA8, GL_RGBA16, GL_RGBA16, GL_RGBA32F, GL_RGBA32F, GL_RGBA16F, GL_RGBA32F}
};

static constexpr GLint swizzle_mask_lut[][4] =
{
	{GL_RED, GL_RED, GL_RED, GL_ONE},
	{GL_RED, GL_GREEN, GL_ZERO, GL_ONE},
	{GL_RED, GL_RED, GL_RED, GL_ONE},
	{GL_RED, GL_RED, GL_RED, GL_GREEN},
	{GL_RED, GL_GREEN, GL_BLUE, GL_ONE},
	{GL_RED, GL_GREEN, GL_BLUE, GL_ONE},
	{GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA},
	{GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA}
};

static constexpr GLenum wrapping_lut[] =
{
	GL_CLAMP_TO_BORDER,
	GL_CLAMP_TO_EDGE,
	GL_REPEAT,
	GL_MIRRORED_REPEAT
};

static constexpr GLenum min_filter_lut[] =
{
	GL_NEAREST,
	GL_LINEAR,
	GL_NEAREST_MIPMAP_NEAREST,
	GL_LINEAR_MIPMAP_NEAREST,
	GL_NEAREST_MIPMAP_LINEAR,
	GL_LINEAR_MIPMAP_LINEAR
};

static constexpr GLenum mag_filter_lut[] =
{
	GL_NEAREST,
	GL_LINEAR
};

texture::texture(std::uint16_t width, std::uint16_t height, std::uint16_t depth, bool cube, gl::pixel_type type, gl::pixel_format format, gl::color_space color_space, const std::byte* data)
{
	m_gl_texture_target = static_cast<unsigned int>(cube ? GL_TEXTURE_CUBE_MAP : (depth) ? GL_TEXTURE_3D : (height) ? GL_TEXTURE_2D : GL_TEXTURE_1D);
	
	glGenTextures(1, &m_gl_texture_id);
	resize(width, height, depth, type, format, color_space, data);
	set_wrapping(m_wrapping[0], m_wrapping[1], m_wrapping[2]);
	set_filters(std::get<0>(m_filters), std::get<1>(m_filters));
	set_max_anisotropy(m_max_anisotropy);
}

texture::texture(std::uint16_t width, std::uint16_t height, bool cube, gl::pixel_type type, gl::pixel_format format, gl::color_space color_space, const std::byte* data):
	texture(width, height, 0, cube, type, format, color_space, data)
{}

texture::texture(std::uint16_t width, bool cube, gl::pixel_type type, gl::pixel_format format, gl::color_space color_space, const std::byte* data):
	texture(width, 0, 0, cube, type, format, color_space, data)
{}

texture::~texture()
{
	glDeleteTextures(1, &m_gl_texture_id);
}

void texture::read(std::span<std::byte> data, gl::pixel_type type, gl::pixel_format format, std::uint8_t level) const
{
	const GLenum gl_format = pixel_format_lut[std::to_underlying(format)];
	const GLenum gl_type = pixel_type_lut[std::to_underlying(type)];
	
	glBindTexture(m_gl_texture_target, m_gl_texture_id);
	glGetTexImage(m_gl_texture_target, static_cast<GLint>(level), gl_format, gl_type, data.data());
}

void texture::set_filters(texture_min_filter min_filter, texture_mag_filter mag_filter)
{
	m_filters = {min_filter, mag_filter};
	
	const GLenum gl_min_filter = min_filter_lut[std::to_underlying(min_filter)];
	const GLenum gl_mag_filter = mag_filter_lut[std::to_underlying(mag_filter)];
	
	glBindTexture(m_gl_texture_target, m_gl_texture_id);
	glTexParameteri(m_gl_texture_target, GL_TEXTURE_MIN_FILTER, gl_min_filter);
	glTexParameteri(m_gl_texture_target, GL_TEXTURE_MAG_FILTER, gl_mag_filter);
}

void texture::set_min_filter(texture_min_filter filter)
{
	const GLenum gl_min_filter = min_filter_lut[std::to_underlying(filter)];
	
	glBindTexture(m_gl_texture_target, m_gl_texture_id);
	glTexParameteri(m_gl_texture_target, GL_TEXTURE_MIN_FILTER, gl_min_filter);
}

void texture::set_mag_filter(texture_mag_filter filter)
{
	const GLenum gl_mag_filter = mag_filter_lut[std::to_underlying(filter)];
	
	glBindTexture(m_gl_texture_target, m_gl_texture_id);
	glTexParameteri(m_gl_texture_target, GL_TEXTURE_MAG_FILTER, gl_mag_filter);
}

void texture::set_base_level(std::uint8_t level)
{
	m_base_level = level;
	
	glBindTexture(m_gl_texture_target, m_gl_texture_id);
	glTexParameteri(m_gl_texture_target, GL_TEXTURE_BASE_LEVEL, static_cast<GLint>(m_base_level));
}

void texture::set_max_level(std::uint8_t level)
{
	m_max_level = level;
	
	glBindTexture(m_gl_texture_target, m_gl_texture_id);
	glTexParameteri(m_gl_texture_target, GL_TEXTURE_MAX_LEVEL, static_cast<GLint>(m_max_level));
}

void texture::set_mipmap_range(std::uint8_t base_level, std::uint8_t max_level)
{
	m_base_level = base_level;
	m_max_level = max_level;
	
	glBindTexture(m_gl_texture_target, m_gl_texture_id);
	glTexParameteri(m_gl_texture_target, GL_TEXTURE_BASE_LEVEL, static_cast<GLint>(m_base_level));
	glTexParameteri(m_gl_texture_target, GL_TEXTURE_MAX_LEVEL, static_cast<GLint>(m_max_level));
}

void texture::set_max_anisotropy(float anisotropy)
{
	m_max_anisotropy = std::max<float>(0.0f, std::min<float>(1.0f, anisotropy));
	
	// Get the maximum supported anisotropy value
	float gl_max_texture_max_anisotropy;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &gl_max_texture_max_anisotropy);
	
	// Lerp between 1.0 and GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT
	float gl_max_anisotropy = 1.0f + m_max_anisotropy * (gl_max_texture_max_anisotropy - 1.0f);
	
	glBindTexture(m_gl_texture_target, m_gl_texture_id);
	glTexParameterf(m_gl_texture_target, GL_TEXTURE_MAX_ANISOTROPY_EXT, gl_max_anisotropy);
}

void texture::set_wrapping(gl::texture_wrapping wrap_s, gl::texture_wrapping wrap_t, gl::texture_wrapping wrap_r)
{
	m_wrapping = {wrap_s, wrap_t, wrap_r};
	
	GLenum gl_wrap_s = wrapping_lut[std::to_underlying(wrap_s)];
	GLenum gl_wrap_t = wrapping_lut[std::to_underlying(wrap_t)];
	GLenum gl_wrap_r = wrapping_lut[std::to_underlying(wrap_r)];
	
	glBindTexture(m_gl_texture_target, m_gl_texture_id);
	glTexParameteri(m_gl_texture_target, GL_TEXTURE_WRAP_S, gl_wrap_s);
	glTexParameteri(m_gl_texture_target, GL_TEXTURE_WRAP_T, gl_wrap_t);
	glTexParameteri(m_gl_texture_target, GL_TEXTURE_WRAP_R, gl_wrap_r);
}

void texture::set_wrapping(gl::texture_wrapping wrap_s, gl::texture_wrapping wrap_t)
{
	std::get<0>(m_wrapping) = wrap_s;
	std::get<1>(m_wrapping) = wrap_t;
	
	GLenum gl_wrap_s = wrapping_lut[std::to_underlying(wrap_s)];
	GLenum gl_wrap_t = wrapping_lut[std::to_underlying(wrap_t)];
	
	glBindTexture(m_gl_texture_target, m_gl_texture_id);
	glTexParameteri(m_gl_texture_target, GL_TEXTURE_WRAP_S, gl_wrap_s);
	glTexParameteri(m_gl_texture_target, GL_TEXTURE_WRAP_T, gl_wrap_t);
}

void texture::set_wrapping(gl::texture_wrapping wrap_s)
{
	std::get<0>(m_wrapping) = wrap_s;
	
	GLenum gl_wrap_s = wrapping_lut[std::to_underlying(wrap_s)];
	
	glBindTexture(m_gl_texture_target, m_gl_texture_id);
	glTexParameteri(m_gl_texture_target, GL_TEXTURE_WRAP_S, gl_wrap_s);
}

void texture::resize(std::uint16_t width, std::uint16_t height, std::uint16_t depth, gl::pixel_type type, gl::pixel_format format, gl::color_space color_space, const std::byte* data)
{
	m_dimensions = {width, height, depth};
	m_pixel_type = type;
	m_pixel_format = format;
	m_color_space = color_space;

	GLenum gl_internal_format;
	if (m_color_space == gl::color_space::srgb)
	{
		gl_internal_format = srgb_internal_format_lut[std::to_underlying(format)][std::to_underlying(type)];
	}
	else
	{
		gl_internal_format = linear_internal_format_lut[std::to_underlying(format)][std::to_underlying(type)];
	}
	
	GLenum gl_format = pixel_format_lut[std::to_underlying(format)];
	const GLint* gl_swizzle_mask = swizzle_mask_lut[std::to_underlying(format)];
	
	GLenum gl_type = pixel_type_lut[std::to_underlying(type)];
	
	// Special cases for depth + stencil pixel formats
	if (gl_internal_format == GL_DEPTH24_STENCIL8)
	{
		gl_type = GL_UNSIGNED_INT_24_8;
	}
	else if (gl_internal_format == GL_DEPTH32F_STENCIL8)
	{
		gl_type = GL_FLOAT_32_UNSIGNED_INT_24_8_REV;
	}
	
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	
	glBindTexture(m_gl_texture_target, m_gl_texture_id);
	
	switch (m_gl_texture_target)
	{
		case GL_TEXTURE_1D:
			glTexImage1D(m_gl_texture_target, 0, gl_internal_format, width, 0, gl_format, gl_type, data);
			break;
		
		case GL_TEXTURE_2D:
			glTexImage2D(m_gl_texture_target, 0, gl_internal_format, width, height, 0, gl_format, gl_type, data);
			break;
		
		case GL_TEXTURE_3D:
			glTexImage3D(m_gl_texture_target, 0, gl_internal_format, width, height, depth, 0, gl_format, gl_type, data);
			break;
		
		case GL_TEXTURE_CUBE_MAP:
			update_cube_faces(gl_internal_format, gl_format, gl_type, data);
			break;
		
		default:
			break;
	}
	
	glGenerateMipmap(m_gl_texture_target);
	
	glTexParameteriv(m_gl_texture_target, GL_TEXTURE_SWIZZLE_RGBA, gl_swizzle_mask);
	
	/// @TODO: remove this
	if (format == pixel_format::d)
	{
		glTexParameteri(m_gl_texture_target, GL_TEXTURE_COMPARE_FUNC, GL_LESS);
		glTexParameteri(m_gl_texture_target, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	}
}

void texture::resize(std::uint16_t width, std::uint16_t height, gl::pixel_type type, gl::pixel_format format, gl::color_space color_space, const std::byte* data)
{
	resize(width, height, 0, type, format, color_space, data);
}

void texture::resize(std::uint16_t width, gl::pixel_type type, gl::pixel_format format, gl::color_space color_space, const std::byte* data)
{
	resize(width, 0, 0, type, format, color_space, data);
}

void texture::update_cube_faces(unsigned int gl_internal_format, unsigned int gl_format, unsigned int gl_type, const std::byte* data)
{
	const auto width = get_width();
	const auto height = get_height();
	const auto layout = texture_cube::infer_cube_map_layout(width, height);
	const auto face_size = texture_cube::infer_cube_map_face_size(layout, width, height);
	
	if (!data)
	{
		for (int i = 0; i < 6; ++i)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, gl_internal_format, face_size, face_size, 0, gl_format, gl_type, nullptr);
		}
		
		return;
	}
	
	std::size_t channel_count = 0;
	switch (m_pixel_format)
	{
		case pixel_format::d:
		case pixel_format::r:
			channel_count = 1;
			break;
		
		case pixel_format::ds:
		case pixel_format::rg:
			channel_count = 2;
			break;
		
		case pixel_format::rgb:
		case pixel_format::bgr:
			channel_count = 3;
			break;
		
		case pixel_format::rgba:
		case pixel_format::bgra:
			channel_count = 4;
			break;
		
		default:
			break;
	}
	
	std::size_t channel_size = 0;
	switch (m_pixel_type)
	{
		case pixel_type::int_8:
		case pixel_type::uint_8:
			channel_size = 1;
			break;
		
		case pixel_type::int_16:
		case pixel_type::uint_16:
		case pixel_type::float_16:
			channel_size = 2;
			break;
		
		case pixel_type::int_32:
		case pixel_type::uint_32:
		case pixel_type::float_32:
			channel_size = 4;
			break;
		
		default:
			break;
	}
	
	const std::size_t pixel_stride = channel_count * channel_size;
	const std::size_t row_stride = static_cast<std::size_t>(face_size) * pixel_stride;
	const std::size_t face_stride = static_cast<std::size_t>(face_size) * row_stride;
	
	constexpr std::uint16_t vcross_offsets[6][2] =
	{
		{2, 2}, {0, 2},
		{1, 3}, {1, 1},
		{1, 0}, {1, 2}
	};
	constexpr std::uint16_t hcross_offsets[6][2] =
	{
		{2, 1}, {0, 1},
		{1, 2}, {1, 0},
		{3, 1}, {1, 1}
	};
	
	std::vector<std::byte> face_buffer(face_stride);
	
	switch (layout)
	{
		case cube_map_layout::column:
			for (std::uint16_t i = 0; i < 6; ++i)
			{
				const std::byte* face_data = data + face_stride * (5 - i);
				
				for (std::uint16_t y = 0; y < face_size; ++y)
				{
					for (std::uint16_t x = 0; x < face_size; ++x)
					{
						if (i < 2 || i > 3)
						{
							std::memcpy(face_buffer.data() + y * row_stride + x * pixel_stride, face_data + (face_size - y - 1) * row_stride + (face_size - x - 1) * pixel_stride, pixel_stride);
						}
						else
						{
							std::memcpy(face_buffer.data() + y * row_stride + x * pixel_stride, face_data + y * row_stride + x * pixel_stride, pixel_stride);
						}
					}
				}
				
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, gl_internal_format, face_size, face_size, 0, gl_format, gl_type, face_buffer.data());
			}
			break;
		
		case cube_map_layout::row:
			for (std::uint16_t i = 0; i < 6; ++i)
			{
				for (std::uint16_t y = 0; y < face_size; ++y)
				{
					for (std::uint16_t x = 0; x < face_size; ++x)
					{
						if (i < 2 || i > 3)
						{
							std::memcpy(face_buffer.data() + y * row_stride + x * pixel_stride, data + row_stride * (face_size - y - 1) * 6 + row_stride * i + (face_size - x - 1) * pixel_stride, pixel_stride);
						}
						else
						{
							std::memcpy(face_buffer.data() + y * row_stride + x * pixel_stride, data + row_stride * y * 6 + row_stride * i + x * pixel_stride, pixel_stride);
						}
					}
				}
				
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, gl_internal_format, face_size, face_size, 0, gl_format, gl_type, face_buffer.data());
			}
			break;
		
		case cube_map_layout::vertical_cross:
			for (std::uint16_t i = 0; i < 6; ++i)
			{
				const std::byte* face_data = data + vcross_offsets[i][1] * row_stride * face_size * 3 + vcross_offsets[i][0] * row_stride;
				
				for (std::uint16_t y = 0; y < face_size; ++y)
				{
					for (std::uint16_t x = 0; x < face_size; ++x)
					{
						if (i < 2 || i > 3)
						{
							std::memcpy(face_buffer.data() + y * row_stride + x * pixel_stride, face_data + (face_size - y - 1) * row_stride * 3 + (face_size - x - 1) * pixel_stride, pixel_stride);
						}
						else
						{
							std::memcpy(face_buffer.data() + y * row_stride + x * pixel_stride, face_data + y * row_stride * 3 + x * pixel_stride, pixel_stride);
						}
					}
				}
				
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, gl_internal_format, face_size, face_size, 0, gl_format, gl_type, face_buffer.data());
			}
			break;
		
		case cube_map_layout::horizontal_cross:
			for (std::uint16_t i = 0; i < 6; ++i)
			{
				const std::byte* face_data = data + hcross_offsets[i][1] * row_stride * face_size * 4 + hcross_offsets[i][0] * row_stride;
				
				for (std::uint16_t y = 0; y < face_size; ++y)
				{
					for (std::uint16_t x = 0; x < face_size; ++x)
					{
						if (i < 2 || i > 3)
						{
							std::memcpy(face_buffer.data() + y * row_stride + x * pixel_stride, face_data + (face_size - y - 1) * row_stride * 4 + (face_size - x - 1) * pixel_stride, pixel_stride);
						}
						else
						{
							std::memcpy(face_buffer.data() + y * row_stride + x * pixel_stride, face_data + y * row_stride * 4 + x * pixel_stride, pixel_stride);
						}
					}
				}
				
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, gl_internal_format, face_size, face_size, 0, gl_format, gl_type, face_buffer.data());
			}
			break;
		
		default:
			throw std::runtime_error("Unsupported cube map layout");
	}
}

} // namespace gl

template <>
std::unique_ptr<gl::texture_1d> resource_loader<gl::texture_1d>::load(::resource_manager& resource_manager, deserialize_context& ctx)
{
	// Load JSON data
	auto json_data = resource_loader<nlohmann::json>::load(resource_manager, ctx);
	
	// Read image filename
	std::string image_filename;
	if (auto element = json_data->find("image"); element != json_data->end())
	{
		image_filename = element.value().get<std::string>();
	}
	
	// Load image
	auto image = resource_manager.load<::image>(image_filename);
	
	// Read color space
	gl::color_space color_space = gl::color_space::linear;
	if (auto element = json_data->find("color_space"); element != json_data->end())
	{
		std::string value = element.value().get<std::string>();
		if (value == "linear")
		{
			color_space = gl::color_space::linear;
		}
		else if (value == "srgb")
		{
			color_space = gl::color_space::srgb;
		}
	}
	
	// Read extension mode
	gl::texture_wrapping wrapping = gl::texture_wrapping::repeat;
	if (auto element = json_data->find("extension"); element != json_data->end())
	{
		std::string value = element.value().get<std::string>();
		if (value == "clip")
		{
			wrapping = gl::texture_wrapping::clip;
		}
		else if (value == "extend")
		{
			wrapping = gl::texture_wrapping::extend;
		}
		else if (value == "repeat")
		{
			wrapping = gl::texture_wrapping::repeat;
		}
		else if (value == "mirrored_repeat")
		{
			wrapping = gl::texture_wrapping::mirrored_repeat;
		}
	}
	
	// Read interpolation mode
	gl::texture_min_filter min_filter = gl::texture_min_filter::linear_mipmap_linear;
	gl::texture_mag_filter mag_filter = gl::texture_mag_filter::linear;
	if (auto element = json_data->find("interpolation"); element != json_data->end())
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
	if (auto element = json_data->find("max_anisotropy"); element != json_data->end())
	{
		max_anisotropy = element.value().get<float>();
	}
	
	// Determine pixel type
	gl::pixel_type type = (image->component_size() == sizeof(float)) ? gl::pixel_type::float_32 : gl::pixel_type::uint_8;

	// Determine pixel format
	gl::pixel_format format;
	if (image->channel_count() == 1)
	{
		format = gl::pixel_format::r;
	}
	else if (image->channel_count() == 2)
	{
		format = gl::pixel_format::rg;
	}
	else if (image->channel_count() == 3)
	{
		format = gl::pixel_format::rgb;
	}
	else if (image->channel_count() == 4)
	{
		format = gl::pixel_format::rgba;
	}
	else
	{
		throw std::runtime_error(std::format("Texture image has unsupported number of channels ({})", image->channel_count()));
	}
	
	// Create texture
	auto texture = std::make_unique<gl::texture_1d>(image->width(), type, format, color_space, image->data());
	texture->set_wrapping(wrapping);
	texture->set_filters(min_filter, mag_filter);
	texture->set_max_anisotropy(max_anisotropy);
	
	return texture;
}

template <>
std::unique_ptr<gl::texture_2d> resource_loader<gl::texture_2d>::load(::resource_manager& resource_manager, deserialize_context& ctx)
{
	// Load JSON data
	auto json_data = resource_loader<nlohmann::json>::load(resource_manager, ctx);
	
	// Read image filename
	std::string image_filename;
	if (auto element = json_data->find("image"); element != json_data->end())
	{
		image_filename = element.value().get<std::string>();
	}
	
	// Load image
	auto image = resource_manager.load<::image>(image_filename);
	
	// Read color space
	gl::color_space color_space = gl::color_space::linear;
	if (auto element = json_data->find("color_space"); element != json_data->end())
	{
		std::string value = element.value().get<std::string>();
		if (value == "linear")
		{
			color_space = gl::color_space::linear;
		}
		else if (value == "srgb")
		{
			color_space = gl::color_space::srgb;
		}
	}
	
	// Read extension mode
	gl::texture_wrapping wrapping = gl::texture_wrapping::repeat;
	if (auto element = json_data->find("extension"); element != json_data->end())
	{
		std::string value = element.value().get<std::string>();
		if (value == "clip")
		{
			wrapping = gl::texture_wrapping::clip;
		}
		else if (value == "extend")
		{
			wrapping = gl::texture_wrapping::extend;
		}
		else if (value == "repeat")
		{
			wrapping = gl::texture_wrapping::repeat;
		}
		else if (value == "mirrored_repeat")
		{
			wrapping = gl::texture_wrapping::mirrored_repeat;
		}
	}
	
	// Read interpolation mode
	gl::texture_min_filter min_filter = gl::texture_min_filter::linear_mipmap_linear;
	gl::texture_mag_filter mag_filter = gl::texture_mag_filter::linear;
	if (auto element = json_data->find("interpolation"); element != json_data->end())
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
	if (auto element = json_data->find("max_anisotropy"); element != json_data->end())
	{
		max_anisotropy = element.value().get<float>();
	}
	
	// Determine pixel type
	gl::pixel_type type = (image->component_size() == sizeof(float)) ? gl::pixel_type::float_32 : gl::pixel_type::uint_8;

	// Determine pixel format
	gl::pixel_format format;
	if (image->channel_count() == 1)
	{
		format = gl::pixel_format::r;
	}
	else if (image->channel_count() == 2)
	{
		format = gl::pixel_format::rg;
	}
	else if (image->channel_count() == 3)
	{
		format = gl::pixel_format::rgb;
	}
	else if (image->channel_count() == 4)
	{
		format = gl::pixel_format::rgba;
	}
	else
	{
		throw std::runtime_error(std::format("Texture image has unsupported number of channels ({})", image->channel_count()));
	}
	
	// Create texture
	auto texture = std::make_unique<gl::texture_2d>(image->width(), image->height(), type, format, color_space, image->data());
	texture->set_wrapping(wrapping, wrapping);
	texture->set_filters(min_filter, mag_filter);
	texture->set_max_anisotropy(max_anisotropy);
	
	return texture;
}

template <>
std::unique_ptr<gl::texture_3d> resource_loader<gl::texture_3d>::load(::resource_manager& resource_manager, deserialize_context& ctx)
{
	throw std::runtime_error("3D texture loading not yet supported");
}

template <>
std::unique_ptr<gl::texture_cube> resource_loader<gl::texture_cube>::load(::resource_manager& resource_manager, deserialize_context& ctx)
{
	// Load JSON data
	auto json_data = resource_loader<nlohmann::json>::load(resource_manager, ctx);
	
	// Read image filename
	std::string image_filename;
	if (auto element = json_data->find("image"); element != json_data->end())
	{
		image_filename = element.value().get<std::string>();
	}
	
	// Load image
	auto image = resource_manager.load<::image>(image_filename);
	
	// Read color space
	gl::color_space color_space = gl::color_space::linear;
	if (auto element = json_data->find("color_space"); element != json_data->end())
	{
		std::string value = element.value().get<std::string>();
		if (value == "linear")
		{
			color_space = gl::color_space::linear;
		}
		else if (value == "srgb")
		{
			color_space = gl::color_space::srgb;
		}
	}
	
	// Read extension mode
	gl::texture_wrapping wrapping = gl::texture_wrapping::repeat;
	if (auto element = json_data->find("extension"); element != json_data->end())
	{
		std::string value = element.value().get<std::string>();
		if (value == "clip")
		{
			wrapping = gl::texture_wrapping::clip;
		}
		else if (value == "extend")
		{
			wrapping = gl::texture_wrapping::extend;
		}
		else if (value == "repeat")
		{
			wrapping = gl::texture_wrapping::repeat;
		}
		else if (value == "mirrored_repeat")
		{
			wrapping = gl::texture_wrapping::mirrored_repeat;
		}
	}
	
	// Read interpolation mode
	gl::texture_min_filter min_filter = gl::texture_min_filter::linear_mipmap_linear;
	gl::texture_mag_filter mag_filter = gl::texture_mag_filter::linear;
	if (auto element = json_data->find("interpolation"); element != json_data->end())
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
	if (auto element = json_data->find("max_anisotropy"); element != json_data->end())
	{
		max_anisotropy = element.value().get<float>();
	}
	
	// Determine pixel type
	gl::pixel_type type = (image->component_size() == sizeof(float)) ? gl::pixel_type::float_32 : gl::pixel_type::uint_8;

	// Determine pixel format
	gl::pixel_format format;
	if (image->channel_count() == 1)
	{
		format = gl::pixel_format::r;
	}
	else if (image->channel_count() == 2)
	{
		format = gl::pixel_format::rg;
	}
	else if (image->channel_count() == 3)
	{
		format = gl::pixel_format::rgb;
	}
	else if (image->channel_count() == 4)
	{
		format = gl::pixel_format::rgba;
	}
	else
	{
		throw std::runtime_error(std::format("Texture image has unsupported number of channels ({})", image->channel_count()));
	}
	
	// Create texture
	auto texture = std::make_unique<gl::texture_cube>(image->width(), image->height(), type, format, color_space, image->data());
	texture->set_wrapping(wrapping, wrapping, wrapping);
	texture->set_filters(min_filter, mag_filter);
	texture->set_max_anisotropy(max_anisotropy);
	
	return texture;
}

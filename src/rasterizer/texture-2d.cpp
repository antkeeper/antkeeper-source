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

#include "rasterizer/texture-2d.hpp"
#include "rasterizer/texture-wrapping.hpp"
#include "rasterizer/texture-filter.hpp"
#include <glad/glad.h>
#include <algorithm>

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

static constexpr GLenum internal_format_lut[][8] =
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

texture_2d::texture_2d(int width, int height, ::pixel_type type, ::pixel_format format, const void* data):
	gl_texture_id(0),
	dimensions({0, 0}),
	wrapping({texture_wrapping::repeat, texture_wrapping::repeat}),
	filters({texture_min_filter::linear_mipmap_linear, texture_mag_filter::linear}),
	max_anisotropy(0.0f)
{
	glGenTextures(1, &gl_texture_id);
	resize(width, height, type, format, data);
	set_wrapping(std::get<0>(wrapping), std::get<1>(wrapping));
	set_filters(std::get<0>(filters), std::get<1>(filters));
	set_max_anisotropy(max_anisotropy);
}

texture_2d::~texture_2d()
{
	glDeleteTextures(1, &gl_texture_id);
}

void texture_2d::resize(int width, int height, ::pixel_type type, ::pixel_format format, const void* data)
{
	dimensions = {width, height};
	pixel_type = type;
	pixel_format = format;

	GLenum gl_internal_format = internal_format_lut[static_cast<std::size_t>(format)][static_cast<std::size_t>(type)];
	GLenum gl_format = pixel_format_lut[static_cast<std::size_t>(format)];
	const GLint* gl_swizzle_mask = swizzle_mask_lut[static_cast<std::size_t>(format)];
	
	GLenum gl_type = pixel_type_lut[static_cast<std::size_t>(type)];
	
	// Special cases for depth + stencil pixel formats
	if (gl_internal_format == GL_DEPTH24_STENCIL8)
		gl_type = GL_UNSIGNED_INT_24_8;
	else if (gl_internal_format == GL_DEPTH32F_STENCIL8)
		gl_type = GL_FLOAT_32_UNSIGNED_INT_24_8_REV;

	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glBindTexture(GL_TEXTURE_2D, gl_texture_id);
	glTexImage2D(GL_TEXTURE_2D, 0, gl_internal_format, width, height, 0, gl_format, gl_type, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, gl_swizzle_mask);
	
	/// TODO: remove this
	if (format == pixel_format::d)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LESS);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	}
}

void texture_2d::set_wrapping(texture_wrapping wrap_s, texture_wrapping wrap_t)
{
	wrapping = {wrap_s, wrap_t};

	GLenum gl_wrap_s = wrapping_lut[static_cast<std::size_t>(wrap_s)];
	GLenum gl_wrap_t = wrapping_lut[static_cast<std::size_t>(wrap_t)];

	glBindTexture(GL_TEXTURE_2D, gl_texture_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, gl_wrap_s);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, gl_wrap_t);
}

void texture_2d::set_filters(texture_min_filter min_filter, texture_mag_filter mag_filter)
{
	filters = {min_filter, mag_filter};

	GLenum gl_min_filter = min_filter_lut[static_cast<std::size_t>(min_filter)];
	GLenum gl_mag_filter = mag_filter_lut[static_cast<std::size_t>(mag_filter)];

	glBindTexture(GL_TEXTURE_2D, gl_texture_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, gl_min_filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, gl_mag_filter);
}

void texture_2d::set_max_anisotropy(float anisotropy)
{
	this->max_anisotropy = std::max<float>(0.0f, std::min<float>(1.0f, anisotropy));

	// Lerp between 1.0f and GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT
	float gl_max_texture_max_anisotropy;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &gl_max_texture_max_anisotropy);
	float gl_max_anisotropy = 1.0f + this->max_anisotropy * (gl_max_texture_max_anisotropy - 1.0f);

	glBindTexture(GL_TEXTURE_2D, gl_texture_id);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, gl_max_anisotropy);
}


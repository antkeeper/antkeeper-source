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

#include <engine/gl/texture-3d.hpp>

namespace gl {

texture_3d::texture_3d(std::uint16_t width, std::uint16_t height, std::uint16_t depth, gl::pixel_type type, gl::pixel_format format, gl::color_space color_space, const std::byte* data):
	texture(width, height, depth, type, format, color_space, data)
{}

texture_3d::~texture_3d()
{}

void texture_3d::resize(std::uint16_t width, std::uint16_t height, std::uint16_t depth, gl::pixel_type type, gl::pixel_format format, gl::color_space color_space, const std::byte* data)
{
	texture::resize(width, height, depth, type, format, color_space, data);
}

void texture_3d::set_wrapping(gl::texture_wrapping wrap_s, texture_wrapping wrap_t, texture_wrapping wrap_r)
{
	texture::set_wrapping(wrap_s, wrap_t, wrap_r);
}

} // namespace gl

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

#include <engine/gl/texture-1d.hpp>

namespace gl {

texture_1d::texture_1d(std::uint16_t width, gl::pixel_type type, gl::pixel_format format, gl::transfer_function transfer_function, const std::byte* data):
	texture(width, false, type, format, transfer_function, data)
{}

void texture_1d::resize(std::uint16_t width, gl::pixel_type type, gl::pixel_format format, gl::transfer_function transfer_function, const std::byte* data)
{
	texture::resize(width, type, format, transfer_function, data);
}

void texture_1d::set_wrapping(gl::texture_wrapping wrap_s)
{
	texture::set_wrapping(wrap_s);
}

} // namespace gl

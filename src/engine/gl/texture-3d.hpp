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

#ifndef ANTKEEPER_GL_TEXTURE_3D_HPP
#define ANTKEEPER_GL_TEXTURE_3D_HPP

#include <engine/gl/texture.hpp>

namespace gl {

/**
 * A 3D texture which can be uploaded to shaders via shader inputs.
 */
class texture_3d: public texture
{
public:
	/// @copydoc texture::texture(std::uint16_t, std::uint16_t, std::uint16_t, gl::pixel_type, gl::pixel_format, gl::color_space, const std::byte*)
	texture_3d(std::uint16_t width, std::uint16_t height, std::uint16_t depth, gl::pixel_type type = gl::pixel_type::uint_8, gl::pixel_format format = gl::pixel_format::rgba, gl::color_space color_space = gl::color_space::linear, const std::byte* data = nullptr);
	
	/// Destructs a 3D texture.
	virtual ~texture_3d();
	
	/// @copydoc texture::resize(std::uint16_t, std::uint16_t, std::uint16_t, gl::pixel_type, gl::pixel_format, gl::color_space, const std::byte*)
	virtual void resize(std::uint16_t width, std::uint16_t height, std::uint16_t depth, gl::pixel_type type, gl::pixel_format format, gl::color_space color_space, const std::byte* data);
	
	/// @copydoc texture::set_wrapping(gl::texture_wrapping, gl::texture_wrapping, gl::texture_wrapping)
	virtual void set_wrapping(gl::texture_wrapping wrap_s, texture_wrapping wrap_t, texture_wrapping wrap_r);
};

} // namespace gl

#endif // ANTKEEPER_GL_TEXTURE_3D_HPP

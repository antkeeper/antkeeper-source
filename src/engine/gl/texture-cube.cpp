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

#include <engine/gl/texture-cube.hpp>
#include <cmath>

namespace gl {

cube_map_layout texture_cube::infer_cube_map_layout(std::uint16_t w, std::uint16_t h) noexcept
{
	if (h == w * 6)
	{
		return cube_map_layout::column;
	}
	else if (w == h * 6)
	{
		return cube_map_layout::row;
	}
	else if (w == (h / 4) * 3)
	{
		return cube_map_layout::vertical_cross;
	}
	else if (h == (w / 4) * 3)
	{
		return cube_map_layout::horizontal_cross;
	}
	else if (w == h * 2)
	{
		return cube_map_layout::equirectangular;
	}
	else if (w == h)
	{
		return cube_map_layout::spherical;
	}
	
	return {};
}

std::uint16_t texture_cube::infer_cube_map_face_size(cube_map_layout layout, std::uint16_t w, std::uint16_t h) noexcept
{
	switch (layout)
	{
		case cube_map_layout::column:
		case cube_map_layout::spherical:
			return w;
		
		case cube_map_layout::row:
			return h;
		
		case cube_map_layout::vertical_cross:
			return h / 4;
		
		case cube_map_layout::horizontal_cross:
		case cube_map_layout::equirectangular:
			return w / 4;
		
		default:
			return 0;
	}
}

texture_cube::texture_cube(std::uint16_t width, std::uint16_t height, gl::pixel_type type, gl::pixel_format format, gl::color_space color_space, const std::byte* data):
	texture(width, height, true, type, format, color_space, data)
{
	resized();
}

void texture_cube::resize(std::uint16_t width, std::uint16_t height, gl::pixel_type type, gl::pixel_format format, gl::color_space color_space, const std::byte* data)
{
	texture::resize(width, height, type, format, color_space, data);
	resized();
}

void texture_cube::resize(std::uint16_t width, std::uint16_t height, const std::byte* data)
{
	texture::resize(width, height, get_pixel_type(), get_pixel_format(), get_color_space(), data);
	resized();
}

void texture_cube::set_wrapping(gl::texture_wrapping wrap_s, texture_wrapping wrap_t, texture_wrapping wrap_r)
{
	texture::set_wrapping(wrap_s, wrap_t, wrap_r);
}

void texture_cube::resized()
{
	const auto w = get_width();
	const auto h = get_height();
	const auto layout = infer_cube_map_layout(w, h);
	m_face_size = infer_cube_map_face_size(layout, w, h);
	m_mip_count = 1 + static_cast<std::uint16_t>(std::log2(m_face_size));
}

} // namespace gl

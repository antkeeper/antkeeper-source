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

#include <engine/gl/cube-map.hpp>

namespace gl {

cube_map_layout infer_cube_map_layout(std::uint32_t width, std::uint32_t height) noexcept
{
	if (width == height / 6)
	{
		return cube_map_layout::column;
	}
	else if (width == height * 6)
	{
		return cube_map_layout::row;
	}
	else if (width == (height / 4) * 3)
	{
		return cube_map_layout::vertical_cross;
	}
	else if (width == (height * 4) / 3)
	{
		return cube_map_layout::horizontal_cross;
	}
	else if (width == height * 2)
	{
		return cube_map_layout::equirectangular;
	}
	else if (width == height)
	{
		return cube_map_layout::spherical;
	}
	
	return cube_map_layout::unknown;
}

std::uint32_t infer_cube_map_face_width(std::uint32_t width, std::uint32_t height, cube_map_layout layout) noexcept
{
	switch (layout)
	{
		case cube_map_layout::column:
		case cube_map_layout::spherical:
			return width;
		
		case cube_map_layout::row:
			return height;
		
		case cube_map_layout::vertical_cross:
			return height / 4;
		
		case cube_map_layout::horizontal_cross:
		case cube_map_layout::equirectangular:
			return width / 4;
		
		default:
			return 0;
	}
}

} // namespace gl

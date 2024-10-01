// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

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
			[[fallthrough]];
		case cube_map_layout::spherical:
			return width;
		
		case cube_map_layout::row:
			return height;
		
		case cube_map_layout::vertical_cross:
			return height / 4;
		
		case cube_map_layout::horizontal_cross:
			[[fallthrough]];
		case cube_map_layout::equirectangular:
			return width / 4;
		
		case cube_map_layout::unknown:
			[[fallthrough]];
		default:
			return 0;
	}
}

} // namespace gl

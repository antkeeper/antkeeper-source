// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

import engine.gl.cube_map;
import engine.utility.sized_types;

namespace engine::gl
{
	cube_map_layout infer_cube_map_layout(u32 width, u32 height) noexcept
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

	u32 infer_cube_map_face_width(u32 width, u32 height, cube_map_layout layout) noexcept
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
}

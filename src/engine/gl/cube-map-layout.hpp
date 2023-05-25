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

#ifndef ANTKEEPER_GL_CUBE_MAP_LAYOUT_HPP
#define ANTKEEPER_GL_CUBE_MAP_LAYOUT_HPP

#include <cstdint>

namespace gl {

/// Cube map layout types.
enum class cube_map_layout: std::uint8_t
{
	/// Faces are stored consecutively in a vertical column.
	column = 1,
	
	/// Faces are stored consecutively in a horizontal row.
	row,
	
	/// Faces are stored in a vertical cross.
	vertical_cross,
	
	/// Faces are stored in a horizontal cross.
	horizontal_cross,
	
	/// Faces are stored in an equirectangular projection.
	equirectangular,
	
	/// Faces are stored in a spherical projection.
	spherical
};

} // namespace gl

#endif // ANTKEEPER_GL_CUBE_MAP_LAYOUT_HPP

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

#ifndef ANTKEEPER_GL_CUBE_MAP_HPP
#define ANTKEEPER_GL_CUBE_MAP_HPP

#include <cstdint>

namespace gl {

/// Cube map layouts.
enum class cube_map_layout: std::uint8_t
{
	/// Unknown layout.
	unknown,
	
	/// Faces are stored consecutively in a single column.
	column,
	
	/// Faces are stored consecutively in a single row.
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

/**
 * Infers the layout of a cube map from its dimensions.
 *
 * @param width Cube map width.
 * @param height Cube map height.
 *
 * @return Inferred cube map layout.
 */
[[nodiscard]] cube_map_layout infer_cube_map_layout(std::uint32_t width, std::uint32_t height) noexcept;

/**
 * Infers the width of a cube map face from its dimensons and layout.
 *
 * @param width Cube map width.
 * @param height Cube map height.
 * @param layout Cube map layout.
 *
 * @return Inferred cube map face width.
 */
[[nodiscard]] std::uint32_t infer_cube_map_face_width(std::uint32_t width, std::uint32_t height, cube_map_layout layout) noexcept;

} // namespace gl

#endif // ANTKEEPER_GL_CUBE_MAP_HPP

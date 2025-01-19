// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

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

/// Infers the layout of a cube map from its dimensions.
/// @param width Cube map width.
/// @param height Cube map height.
/// @return Inferred cube map layout.
[[nodiscard]] cube_map_layout infer_cube_map_layout(std::uint32_t width, std::uint32_t height) noexcept;

/// Infers the width of a cube map face from its dimensons and layout.
/// @param width Cube map width.
/// @param height Cube map height.
/// @param layout Cube map layout.
/// @return Inferred cube map face width.
[[nodiscard]] std::uint32_t infer_cube_map_face_width(std::uint32_t width, std::uint32_t height, cube_map_layout layout) noexcept;

} // namespace gl

#endif // ANTKEEPER_GL_CUBE_MAP_HPP

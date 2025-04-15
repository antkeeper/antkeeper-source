// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.gl.cube_map;
import engine.utility.sized_types;

export namespace engine::gl
{
	/// Cube map layouts.
	enum class cube_map_layout: u8
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
	[[nodiscard]] cube_map_layout infer_cube_map_layout(u32 width, u32 height) noexcept;

	/// Infers the width of a cube map face from its dimensons and layout.
	/// @param width Cube map width.
	/// @param height Cube map height.
	/// @param layout Cube map layout.
	/// @return Inferred cube map face width.
	[[nodiscard]] u32 infer_cube_map_face_width(u32 width, u32 height, cube_map_layout layout) noexcept;
}

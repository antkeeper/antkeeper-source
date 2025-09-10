// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <engine/utility/sized-types.hpp>

/// Marching cubes (MC) algorithm functions and constants.
namespace engine::geom::marching_cubes
{
	/// Uses the marching cubes algorithm to polygonize a single cell.
	/// @param[out] vertices Array which can hold at least 12 vertices (36 floats).
	/// @param[out] vertex_count Number of generated vertices.
	/// @param[out] triangles Array which can hold 5 at least triangles (15 ints).
	/// @param[out] triangle_count Number of generated triangles. The maximum number triangles generated for a single cell is 5.
	/// @param[in] corners Array of 8 corners of the cell.
	/// @param[in] distances Array of 8 distances from the corners to the isosurface.
	void polygonize(float* vertices, u8* vertex_count, i8* triangles, u8* triangle_count, const float* corners, const float* distances);

	/// Vertices of a unit cube.
	constexpr float unit_cube[8][3] =
	{
		{0, 0, 0},
		{1, 0, 0},
		{1, 1, 0},
		{0, 1, 0},
		{0, 0, 1},
		{1, 0, 1},
		{1, 1, 1},
		{0, 1, 1}
	};
}

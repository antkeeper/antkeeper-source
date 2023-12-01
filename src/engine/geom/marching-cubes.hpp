// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GEOM_MARCHING_CUBES_HPP
#define ANTKEEPER_GEOM_MARCHING_CUBES_HPP

#include <cstdint>

namespace geom {

/// Marching cubes (MC) algorithm functions and constants.
namespace mc {

/**
 * Uses the marching cubes algorithm to polygonize a single cell.
 *
 * @param[out] vertices Array which can hold at least 12 vertices (36 floats).
 * @param[out] vertex_count Number of generated vertices.
 * @param[out] triangles Array which can hold 5 at least triangles (15 ints).
 * @param[out] triangle_count Number of generated triangles. The maximum number triangles generated for a single cell is 5.
 */
void polygonize(float* vertices, std::uint8_t* vertex_count, std::int8_t* triangles, std::uint8_t* triangle_count, const float* corners, const float* distances);

/**
 * Vertices of a unit cube.
 */
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

} // namespace mc
} // namespace geom

#endif // ANTKEEPER_GEOM_MARCHING_CUBES_HPP

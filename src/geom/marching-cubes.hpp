/*
 * Copyright (C) 2021  Christopher J. Howard
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
void polygonize(float* vertices, std::uint_fast8_t* vertex_count, std::int_fast8_t* triangles, std::uint_fast8_t* triangle_count, const float* corners, const float* distances);

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


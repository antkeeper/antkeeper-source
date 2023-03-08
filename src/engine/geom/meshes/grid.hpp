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

#ifndef ANTKEEPER_GEOM_MESHES_GRID_HPP
#define ANTKEEPER_GEOM_MESHES_GRID_HPP

#include <engine/geom/mesh.hpp>
#include <memory>

namespace geom {
namespace meshes {

/**
 * Generates a grid mesh on the XY plane.
 *
 * @param length Side length of the grid.
 * @param subdivisions_x Number of subdivisions on the x-axis.
 * @param subdivisions_y Number of subdivisions on the y-axis.
 * @return Grid mesh on the XY plane.
 */
[[nodiscard]] std::unique_ptr<geom::mesh> grid_xy(float length, std::size_t subdivisions_x, std::size_t subdivisions_y);

} // namespace meshes
} // namespace geom

#endif // ANTKEEPER_GEOM_MESHES_GRID_HPP

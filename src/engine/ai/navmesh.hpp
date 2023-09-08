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

#ifndef ANTKEEPER_AI_NAVMESH_HPP
#define ANTKEEPER_AI_NAVMESH_HPP

#include <engine/math/vector.hpp>
#include <engine/math/se3.hpp>
#include <engine/geom/brep/brep-mesh.hpp>
#include <engine/geom/primitives/point.hpp>
#include <engine/geom/primitives/ray.hpp>
#include <engine/geom/coordinates.hpp>
#include <vector>

namespace ai {

struct navmesh_traversal
{
	geom::brep_face* face;
	geom::brep_edge* edge;
	geom::point<float, 3> barycentric;
	
	geom::point<float, 3> target_point;
	geom::point<float, 3> closest_point;
	geom::triangle_region closest_region;
};

/**
 * Moves a point along the surface of a mesh.
 */
[[nodiscard]] navmesh_traversal traverse_navmesh(const geom::brep_mesh& mesh, geom::brep_face* face, const math::fvec3& start, const math::fvec3& end);

} // namespace ai

#endif // ANTKEEPER_AI_NAVMESH_HPP

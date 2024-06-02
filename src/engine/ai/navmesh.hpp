// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_AI_NAVMESH_HPP
#define ANTKEEPER_AI_NAVMESH_HPP

#include <engine/geom/brep/brep-mesh.hpp>
#include <engine/geom/brep/brep-feature.hpp>
#include <engine/geom/primitives/point.hpp>
#include <engine/geom/primitives/ray.hpp>
#include <engine/geom/coordinates.hpp>
#include <engine/math/vector.hpp>
#include <vector>

namespace ai {

struct navmesh_traversal
{
	geom::brep_feature feature;
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

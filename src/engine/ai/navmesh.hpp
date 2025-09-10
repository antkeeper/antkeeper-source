// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <engine/geom/primitives/point.hpp>
#include <engine/geom/coordinates.hpp>
#include <engine/geom/brep/mesh.hpp>
#include <engine/geom/brep/feature.hpp>
#include <engine/math/vector.hpp>

namespace engine::ai
{
	/// Navmesh traversal results.
	struct navmesh_traversal
	{
		engine::geom::brep::feature feature;
		geom::point<float, 3> barycentric;
		geom::point<float, 3> target_point;
		geom::point<float, 3> closest_point;
		geom::triangle_region closest_region;
	};

	/// @fn ai::traverse_navmesh
	/// Moves a point along the surface of a mesh.
	[[nodiscard]] navmesh_traversal traverse_navmesh(const engine::geom::brep::mesh& mesh, engine::geom::brep::face* face, const math::fvec3& start, const math::fvec3& end);
}

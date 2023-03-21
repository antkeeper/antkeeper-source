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

#ifndef ANTKEEPER_GEOM_MESH_ACCELERATOR_HPP
#define ANTKEEPER_GEOM_MESH_ACCELERATOR_HPP

#include <engine/geom/mesh.hpp>
#include <engine/geom/octree.hpp>
#include <engine/geom/primitives/box.hpp>
#include <engine/geom/intersection.hpp>
#include <engine/utility/fundamental-types.hpp>
#include <list>
#include <optional>
#include <unordered_map>

namespace geom {

/**
 * Acceleration structure for querying mesh geometry.
 */
class mesh_accelerator
{
public:
	struct ray_query_result
	{
		float t;
		geom::mesh::face* face;
	};

	mesh_accelerator();

	/**
	 * Builds the acceleration structure.
	 */
	void build(const mesh& mesh);

	/**
	 * Finds the first intersection between a ray and a triangle in the mesh.
	 *
	 * @param ray Ray to test for intersection.
	 * @return Ray query result on intersection, and `std::nullopt` if no intersection occurred.
	 */
	std::optional<ray_query_result> query_nearest(const ray<float, 3>& ray) const;
	
private:
	typedef unordered_octree32 octree_type;
	
	box<float> get_node_bounds(typename octree_type::node_type node) const;

	void query_nearest_recursive(float& nearest_t, geom::mesh::face*& nearest_face, typename octree_type::node_type node, const ray<float, 3>& ray) const;

	/// Returns the max-depth node in which the point is located
	typename octree_type::node_type find_node(const float3& point) const;

	octree_type octree;
	float3 node_dimensions[octree_type::max_depth + 1];
	float3 center_offset;
	std::unordered_map<typename octree_type::node_type, std::list<mesh::face*>> face_map;
};

} // namespace geom

#endif // ANTKEEPER_GEOM_MESH_ACCELERATOR_HPP


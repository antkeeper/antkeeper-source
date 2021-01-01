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

#ifndef ANTKEEPER_MESH_ACCELERATOR_HPP
#define ANTKEEPER_MESH_ACCELERATOR_HPP

#include "geometry/mesh.hpp"
#include "geometry/octree.hpp"
#include "geometry/aabb.hpp"
#include "geometry/intersection.hpp"
#include "utility/fundamental-types.hpp"
#include <list>
#include <optional>
#include <unordered_map>

/**
 * Acceleration structure for querying mesh geometry.
 */
class mesh_accelerator
{
public:
	struct ray_query_result
	{
		float t;
		::mesh::face* face;
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
	 * @return Ray query result on intersection, and `std::nullopt` if no intersection occurreda.
	 */
	std::optional<ray_query_result> query_nearest(const ray<float>& ray) const;
	
private:
	aabb<float> get_node_bounds(octree32::node_type node) const;

	void query_nearest_recursive(float& nearest_t, ::mesh::face*& nearest_face, octree32::node_type node, const ray<float>& ray) const;

	/// Returns the max-depth node in which the point is located
	octree32::node_type find_node(const float3& point) const;

	octree32 octree;
	float3 node_dimensions[octree32::max_depth + 1];
	float3 center_offset;
	std::unordered_map<octree32::node_type, std::list<mesh::face*>> face_map;
};

#endif // ANTKEEPER_MESH_ACCELERATOR_HPP


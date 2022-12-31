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

#include "geom/mesh-accelerator.hpp"
#include "geom/mesh-functions.hpp"
#include "geom/morton.hpp"
#include <bitset>

namespace geom {

mesh_accelerator::mesh_accelerator()
{}

void mesh_accelerator::build(const mesh& mesh)
{
	// Clear octree and face map
	octree.clear();
	face_map.clear();

	// Calculate mesh dimensions
	aabb<float> bounds = calculate_bounds(mesh);
	float3 mesh_dimensions = bounds.max_point - bounds.min_point;
	center_offset = mesh_dimensions * 0.5f - (bounds.min_point + bounds.max_point) * 0.5f;

	// Calculate node dimensions at each octree depth
	for (auto i = 0; i <= octree_type::max_depth; ++i)
	{
		node_dimensions[i] = mesh_dimensions * static_cast<float>((1.0f / std::pow(2, i)));
	}

	// Add faces to octree
	for (mesh::face* face: mesh.get_faces())
	{
		// Calculate face bounds
		float3 min_point = reinterpret_cast<const float3&>(face->edge->vertex->position);
		float3 max_point = min_point;
		mesh::edge* edge = face->edge;
		do
		{
			const auto& position = edge->vertex->position;
			for (int i = 0; i < 3; ++i)
			{
				min_point[i] = std::min<float>(min_point[i], position[i]);
				max_point[i] = std::max<float>(max_point[i], position[i]);
			}

			edge = edge->next;
		}
		while (edge != face->edge);

		// 1. Find max depth node of aabb min
		// 2. Find max depth node of aabb max
		// 3. Find common ancestor of the two nodes--that's the containing node.
		typename octree_type::node_type min_node = find_node(min_point);
		typename octree_type::node_type max_node = find_node(max_point);
		typename octree_type::node_type containing_node = octree_type::common_ancestor(min_node, max_node);

		// Insert containing node into octree
		octree.insert(containing_node);

		// Add face to face map
		face_map[containing_node].push_back(face);
	}
}

std::optional<mesh_accelerator::ray_query_result> mesh_accelerator::query_nearest(const ray<float>& ray) const
{
	ray_query_result result;
	result.t = std::numeric_limits<float>::infinity();
	result.face = nullptr;

	query_nearest_recursive(result.t, result.face, octree.root, ray);

	if (result.face)
		return std::optional{result};
	return std::nullopt;
}

void mesh_accelerator::query_nearest_recursive(float& nearest_t, geom::mesh::face*& nearest_face, typename octree_type::node_type node, const ray<float>& ray) const
{
	// Get node bounds
	const aabb<float> node_bounds = get_node_bounds(node);

	// Test for intersection with node bounds
	auto aabb_intersection = ray_aabb_intersection(ray, node_bounds);

	// If ray passed through this node
	if (std::get<0>(aabb_intersection))
	{
		// Test all triangles in the node
		if (auto it = face_map.find(node); it != face_map.end())
		{
			const std::list<mesh::face*>& faces = it->second;

			for (mesh::face* face: faces)
			{
				// Get triangle coordinates
				const float3& a = reinterpret_cast<const float3&>(face->edge->vertex->position);
				const float3& b = reinterpret_cast<const float3&>(face->edge->next->vertex->position);
				const float3& c = reinterpret_cast<const float3&>(face->edge->previous->vertex->position);

				// Test for intersection with triangle
				auto triangle_intersection = ray_triangle_intersection(ray, a, b, c);
				if (std::get<0>(triangle_intersection))
				{
					float t = std::get<1>(triangle_intersection);
					if (t < nearest_t)
					{
						nearest_t = t;
						nearest_face = face;
					}
				}
			}
		}

		// Test all child nodes
		if (!octree.is_leaf(node))
		{
			for (int i = 0; i < 8; ++i)
				query_nearest_recursive(nearest_t, nearest_face, octree.child(node, i), ray);
		}
	}
}

aabb<float> mesh_accelerator::get_node_bounds(typename octree_type::node_type node) const
{
	// Decode Morton location of node
	std::uint32_t x, y, z;
	morton::decode(octree_type::location(node), x, y, z);
	float3 node_location = float3{static_cast<float>(x), static_cast<float>(y), static_cast<float>(z)};

	// Get node dimensions at node depth
	const float3& dimensions = node_dimensions[octree_type::depth(node)];

	// Calculate AABB
	float3 min_point = (node_location * dimensions) - center_offset;
	return aabb<float>{min_point, min_point + dimensions};
}

typename mesh_accelerator::octree_type::node_type mesh_accelerator::find_node(const float3& point) const
{
	// Transform point to octree space
	float3 transformed_point = (point + center_offset);

	// Account for floating-point tolerance
	const float epsilon = 0.00001f;
	transformed_point.x() = std::max<float>(0.0f, std::min<float>(node_dimensions[0].x() - epsilon, transformed_point.x()));
	transformed_point.y() = std::max<float>(0.0f, std::min<float>(node_dimensions[0].y() - epsilon, transformed_point.y()));
	transformed_point.z() = std::max<float>(0.0f, std::min<float>(node_dimensions[0].z() - epsilon, transformed_point.z()));

	// Transform point to max-depth node space
	transformed_point = transformed_point / node_dimensions[octree_type::max_depth];

	// Encode transformed point as a Morton location code
	std::uint32_t location = morton::encode(
		static_cast<std::uint32_t>(transformed_point.x()),
		static_cast<std::uint32_t>(transformed_point.y()),
		static_cast<std::uint32_t>(transformed_point.z()));
	
	// Return max depth node at the determined location
	return octree_type::node(octree_type::max_depth, location);
}

} // namespace geom

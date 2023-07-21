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

#include <engine/geom/bvh/bvh.hpp>
#include <engine/geom/intersection.hpp>
#include <engine/geom/brep/brep-mesh.hpp>
#include <engine/debug/log.hpp>
#include <algorithm>
#include <numeric>

namespace geom {

bvh::bvh(std::span<const bvh_primitive> primitives)
{
	build(primitives);
}

bvh::bvh(const brep_mesh& mesh)
{
	build(mesh);
}

void bvh::build(std::span<const bvh_primitive> primitives)
{
	if (primitives.empty())
	{
		m_primitive_indices.clear();
		m_nodes.clear();
		m_node_count = 0;
	}
	else
	{
		// Allocate and fill primitive index array
		m_primitive_indices.resize(primitives.size());
		std::iota(m_primitive_indices.begin(), m_primitive_indices.end(), 0);
		
		// Allocate nodes
		m_nodes.resize(primitives.size() << 1);
		
		// Init root node
		m_node_count = 1;
		auto& root = m_nodes.front();
		root.size = static_cast<std::uint32_t>(primitives.size());
		root.offset = 0;
		update_bounds(root, primitives);
		
		// Recursively build BVH
		subdivide(root, primitives);
	}
}

void bvh::build(const brep_mesh& mesh)
{
	// Get mesh vertex positions attribute
	const auto& vertex_positions = mesh.vertices().attributes().at<math::fvec3>("position");
	
	// Allocate BVH primitives for mesh faces
	std::vector<bvh_primitive> primitives(mesh.faces().size());
	
	// Calculate bounding boxes
	for (brep_face* face: mesh.faces())
	{
		auto& primitive = primitives[face->index()];
		primitive.centroid = {};
		primitive.bounds = {math::fvec3::infinity(), -math::fvec3::infinity()};
		
		for (brep_loop* loop: face->loops())
		{
			const auto& vertex_position = vertex_positions[loop->vertex()->index()];
			
			primitive.centroid += vertex_position;
			primitive.bounds.extend(vertex_position);
		}
		
		primitive.centroid /= static_cast<float>(face->loops().size());
	}
	
	// Build BVH from the bounding boxes of the mesh faces
	build(primitives);
}

void bvh::update_bounds(bvh_node& node, const std::span<const bvh_primitive>& primitives)
{
	node.bounds = {math::fvec3::infinity(), -math::fvec3::infinity()};
	for (std::uint32_t i = 0; i < node.size; ++i)
	{
		const auto& primitive = primitives[m_primitive_indices[node.offset + i]];
		node.bounds.extend(primitive.bounds);
	};
}

void bvh::subdivide(bvh_node& node, const std::span<const bvh_primitive>& primitives)
{
	if (node.size <= 2)
	{
		return;
	}
	
	// Determine index of split axis
	const auto extents = node.bounds.size();
	std::uint8_t split_axis = 0;
	if (extents.y() > extents.x())
	{
		split_axis = 1;
	}
	if (extents.z() > extents[split_axis])
	{
		split_axis = 2;
	}
	
	// Determine split coordinate
	const float split_coord = node.bounds.min[split_axis] + extents[split_axis] * 0.5f;
	
	std::uint32_t i = node.offset;
	std::uint32_t j = (node.size) ? i + node.size - 1 : i;
	while (i <= j)
	{
		const auto& primitive = primitives[m_primitive_indices[i]];
		
		if (primitive.centroid[split_axis] < split_coord)
		// if (primitive.bounds.center()[split_axis] < split_coord)
		{
			++i;
		}
		else
		{
			std::swap(m_primitive_indices[i], m_primitive_indices[j]);
			if (!j)
			{
				break;
			}
			--j;
		}
	}
	
	const std::uint32_t left_size = i - node.offset;
	if (!left_size || left_size == node.size)
	{
		return;
	}
	
	const std::uint32_t left_index = m_node_count++;
	auto& left_child = m_nodes[left_index];
	left_child.offset = node.offset;
	left_child.size = left_size;
	update_bounds(left_child, primitives);
	
	const std::uint32_t right_index = m_node_count++;
	auto& right_child = m_nodes[right_index];
	right_child.offset = i;
	right_child.size = node.size - left_size;
	update_bounds(right_child, primitives);
	
	node.offset = left_index;
	node.size = 0;
	subdivide(left_child, primitives);
	subdivide(right_child, primitives);
}

void bvh::visit(const bvh_node& node, const geom::ray<float, 3>& ray, const visitor_type& f) const
{
	if (!geom::intersection(ray, node.bounds))
	{
		return;
	}
	
	if (node.is_leaf())
	{
		// Visit leaf node primitives
		for (std::uint32_t i = 0; i < node.size; ++i)
		{
			f(m_primitive_indices[node.offset + i]);
		}
	}
	else
	{
		// Recursively visit node children
		visit(m_nodes[node.offset], ray, f);
		visit(m_nodes[node.offset + 1], ray, f);
	}
}

} // namespace geom

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

#include <engine/physics/kinematics/colliders/mesh-collider.hpp>
#include <engine/debug/log.hpp>
#include <engine/geom/intersection.hpp>
#include <engine/geom/brep/brep-operations.hpp>
#include <limits>

namespace physics {

mesh_collider::mesh_collider(std::shared_ptr<mesh_type> mesh)
{
	set_mesh(mesh);
}

void mesh_collider::set_mesh(std::shared_ptr<mesh_type> mesh)
{
	m_mesh = mesh;
	if (m_mesh)
	{
		// Store pointer to mesh vertex positions
		m_vertex_positions = &m_mesh->vertices().attributes().at<math::fvec3>("position");
		
		// If mesh has no face normals
		if (!m_mesh->faces().attributes().contains("normal"))
		{
			// Generate face normals
			generate_face_normals(*m_mesh);
		}
		
		/// @TODO: vertex normals aren't needed for mesh colliders, they're generated here for the locomotion system (remove later)
		// If mesh has no vertex normals
		if (!m_mesh->vertices().attributes().contains("normal"))
		{
			// Generate vertex normals
			generate_vertex_normals(*m_mesh);
		}
		
		// Store pointer to mesh face normals
		m_face_normals = &m_mesh->faces().attributes().at<math::fvec3>("normal");
	}
	else
	{
		m_vertex_positions = nullptr;
		m_face_normals = nullptr;
	}
	
	rebuild_bvh();
}

void mesh_collider::rebuild_bvh()
{
	if (m_mesh)
	{
		m_bvh.build(*m_mesh);
	}
	else
	{
		m_bvh.clear();
	}
}

std::optional<std::tuple<float, std::uint32_t, math::fvec3>> mesh_collider::intersection(const geom::ray<float, 3>& ray) const
{
	if (!m_mesh)
	{
		return std::nullopt;
	}
	
	bool triangle_hit = false;
	float nearest_face_distance = std::numeric_limits<float>::infinity();
	std::uint32_t nearest_face_index;
	
	// For each BVH leaf node that intersects ray
	m_bvh.visit
	(
		ray,
		[&](std::uint32_t index)
		{
			// If ray is facing backside of face
			if (math::dot((*m_face_normals)[index], ray.direction) > 0.0f)
			{
				// Ignore face
				return;
			}
			
			// Get pointer to mesh face from BVH primitive index
			geom::brep_face* face = m_mesh->faces()[index];
			
			// Get face vertex positions
			auto loop = face->loops().begin();
			const auto& a = (*m_vertex_positions)[loop->vertex()->index()];
			const auto& b = (*m_vertex_positions)[(++loop)->vertex()->index()];
			const auto& c = (*m_vertex_positions)[(++loop)->vertex()->index()];
			
			// If ray intersects face
			if (const auto intersection = geom::intersection(ray, a, b, c))
			{
				// If distance to point of intersection is nearer than nearest intersection
				float t = std::get<0>(*intersection);
				if (t < nearest_face_distance)
				{
					// Update nearest intersection
					nearest_face_distance = t;
					nearest_face_index = index;
					
					triangle_hit = true;
				}
			}
		}
	);
	
	if (!triangle_hit)
	{
		return std::nullopt;
	}
	
	return std::make_tuple(nearest_face_distance, nearest_face_index, (*m_face_normals)[nearest_face_index]);
}

} // namespace physics

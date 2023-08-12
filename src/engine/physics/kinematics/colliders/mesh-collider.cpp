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
		m_vertex_positions = &m_mesh->vertices().attributes().at<math::fvec3>("position");
	}
	else
	{
		m_vertex_positions = nullptr;
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

std::optional<std::tuple<float, std::uint32_t>> mesh_collider::intersection(const math::transform<float>& mesh_transform, const geom::ray<float, 3>& ray) const
{
	// Transform ray into mesh space
	const auto inv_mesh_transform = math::inverse(mesh_transform);
	const geom::ray<float, 3> mesh_space_ray = 
	{
		inv_mesh_transform * ray.origin,
		inv_mesh_transform.rotation * ray.direction
	};
	
	std::size_t box_hit_count = 0;
	std::size_t triangle_hit_count = 0;
	float nearest_face_distance = std::numeric_limits<float>::infinity();
	std::uint32_t nearest_face_index;
	
	m_bvh.visit
	(
		mesh_space_ray,
		[&](std::uint32_t index)
		{
			++box_hit_count;
			
			geom::brep_face* face = m_mesh->faces()[index];
			auto loop = face->loops().begin();
			const auto& a = (*m_vertex_positions)[loop->vertex()->index()];
			const auto& b = (*m_vertex_positions)[(++loop)->vertex()->index()];
			const auto& c = (*m_vertex_positions)[(++loop)->vertex()->index()];
			
			if (auto intersection = geom::intersection(mesh_space_ray, a, b, c))
			{
				++triangle_hit_count;
				
				float t = std::get<0>(*intersection);
				if (t < nearest_face_distance)
				{
					nearest_face_distance = t;
					nearest_face_index = index;
				}
			}
		}
	);
	
	debug::log::info("mesh collider intersection test:\n\tboxes hit: {}\n\ttriangles hit: {}", box_hit_count, triangle_hit_count);
	
	if (!triangle_hit_count)
	{
		return std::nullopt;
	}
	
	return std::make_tuple(nearest_face_distance, nearest_face_index);
}

} // namespace physics

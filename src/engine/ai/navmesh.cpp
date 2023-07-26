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

#include <engine/ai/navmesh.hpp>
#include <engine/geom/closest-feature.hpp>
#include <engine/geom/coordinates.hpp>
#include <engine/math/quaternion.hpp>
#include <iterator>

namespace ai {

navmesh_traversal traverse_navmesh(const geom::brep_mesh& mesh, geom::brep_face* face, geom::ray<float, 3> ray, float distance)
{
	// Get vertex positions and face normals
	const auto& vertex_positions = mesh.vertices().attributes().at<math::fvec3>("position");
	const auto& face_normals = mesh.faces().attributes().at<math::fvec3>("normal");
	
	// Init traversal result
	navmesh_traversal traversal;
	traversal.edge = nullptr;
	
	// Save initial ray origin
	auto initial_origin = ray.origin;
	
	float segment_length = 0.0f;
	int edge_index;
	do
	{
		// Get triangle vertices
		auto loop_it = face->loops().begin();
		const auto& a = vertex_positions[loop_it->vertex()->index()];
		const auto& b = vertex_positions[(++loop_it)->vertex()->index()];
		const auto& c = vertex_positions[(++loop_it)->vertex()->index()];
		
		// Find closest feature on triangle to segment endpoint
		std::tie(traversal.barycentric, edge_index) = geom::closest_feature(a, b, c, ray.extrapolate(distance));
		
		// Convert barycentric coordinates of closest point to Cartesian coordinates
		traversal.cartesian = geom::barycentric_to_cartesian(traversal.barycentric, a, b, c);
		
		// Subtract length of projected segment from remaining distance
		segment_length = math::length(traversal.cartesian - ray.origin);
		distance -= segment_length;
		
		// Advance ray origin
		ray.origin = traversal.cartesian;
		
		// If no edge reached or no remaining distance, traversal is complete
		if (edge_index < 0 || distance <= 0.0f)
		{
			break;
		}
		
		// Find loop and edge on which the closest point lies
		auto closest_loop_it = face->loops().begin();
		std::advance(closest_loop_it, edge_index);
		geom::brep_loop* closest_loop = *closest_loop_it;
		geom::brep_edge* closest_edge = closest_loop->edge();
		
		// Abort if a boundary edge was reached
		if (closest_edge->loops().size() == 1)
		{
			traversal.edge = closest_edge;
			break;
		}
		
		// Find a loop and face that shares the closest edge
		auto symmetric_loop_it = closest_edge->loops().begin();
		if (*symmetric_loop_it == closest_loop)
		{
			++symmetric_loop_it;
		}
		geom::brep_loop* symmetric_loop = *symmetric_loop_it;
		geom::brep_face* symmetric_face = symmetric_loop->face();
		
		// Find quaternion representing rotation from normal of first face to normal of second face
		const auto& n0 = face_normals[face->index()];
		const auto& n1 = face_normals[symmetric_face->index()];
		const auto rotation = math::rotation(n0, n1);
		
		// Rotate ray direction
		ray.direction = rotation * ray.direction;
		
		// Move to next face
		face = symmetric_face;
	}
	while (segment_length > 0.0f);
	
	traversal.face = face;
	traversal.remaining_distance = distance;
	
	return traversal;
}

} // namespace ai

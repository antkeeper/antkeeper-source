// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

import engine.ai.navmesh;
import engine.math.functions;
import engine.math.quaternion;
import engine.geom.coordinates;
import engine.geom.closest_point;

namespace brep = engine::geom::brep;

namespace engine::ai
{
	navmesh_traversal traverse_navmesh(const brep::mesh& mesh, brep::face* face, const math::fvec3& start, const math::fvec3& end)
	{
		// Get vertex positions and face normals
		const auto& vertex_positions = mesh.vertices().attributes().at<math::fvec3>("position");
		const auto& face_normals = mesh.faces().attributes().at<math::fvec3>("normal");
	
		// Init traversal result
		navmesh_traversal traversal;
	
		geom::triangle_region region;
	
		auto target_point = end;
		auto traversal_direction = math::normalize(end - start);
		math::fvec3 closest_point;
	
		brep::edge* previous_closest_edge{};
	
		do
		{
			// Get vertex positions of face
			auto loop_it = face->loops().begin();
			const auto& a = vertex_positions[loop_it->vertex()->index()];
			const auto& b = vertex_positions[(++loop_it)->vertex()->index()];
			const auto& c = vertex_positions[(++loop_it)->vertex()->index()];
		
			// Find closest point on face to target point
			std::tie(closest_point, region) = geom::closest_point(a, b, c, target_point);
		
			// If point is on the face
			if (geom::is_face_region(region))
			{
				// Traversal complete
				break;
			}
		
			brep::loop* closest_loop;
		
			// If point is on an edge
			if (geom::is_edge_region(region))
			{
				// Get index of the edge
				const auto edge_index = geom::edge_index(region);
			
				// Get pointer to the edge's loop
				loop_it = face->loops().begin();
				std::advance(loop_it, edge_index);
				closest_loop = *loop_it;
			
				// If edge is a boundary edge
				if (closest_loop->edge()->loops().size() == 1)
				{
					// Abort traversal
					traversal.feature = closest_loop->edge();
					break;
				}
			}
			else
			{
				// Point is on a vertex, get index of vertex on which point lies
				const auto vertex_index = geom::vertex_index(region);
			
				// Get pointer to loop originating at the vertex
				loop_it = face->loops().begin();
				std::advance(loop_it, vertex_index);
				brep::loop* loop = *loop_it;
			
				// If previous loop edge is a boundary edge
				if (loop->previous()->edge()->loops().size() == 1)
				{
					// If current loop edge is also a boundary edge
					if (loop->edge()->loops().size() == 1)
					{
						// Abort traversal
						traversal.feature = loop->edge();
						break;
					}
				
					// Select current loop
					closest_loop = loop;
				}
				// If current loop edge is a boundary edge
				else if (loop->edge()->loops().size() == 1)
				{
					// Select previous loop
					closest_loop = loop->previous();
				}
				else
				// Neither loop edge is a boundary edge
				{
					// Calculate direction of current loop edge
					const auto current_direction = math::normalize
					(
						vertex_positions[loop->next()->vertex()->index()] -
						vertex_positions[loop->vertex()->index()]
					);
				
					// Calculate direction of previous loop edge
					const auto previous_direction = math::normalize
					(
						vertex_positions[loop->vertex()->index()] -
						vertex_positions[loop->previous()->vertex()->index()]
					);
				
					// Select loop with minimal angle between edge and traversal direction
					if (math::abs(math::dot(traversal_direction, current_direction)) <
						math::abs(math::dot(traversal_direction, previous_direction)))
					{
						closest_loop = loop;
					}
					else
					{
						closest_loop = loop->previous();
					}
				}
			}
		
			// Get edge of closest loop
			brep::edge* closest_edge = closest_loop->edge();
		
			// If closest edge is previous closest edge
			if (closest_edge == previous_closest_edge)
			{
				// Abort traversal
				traversal.feature = closest_edge;
				break;
			}
		
			// Remember closest edge to prevent infinite loops
			previous_closest_edge = closest_edge;
		
			// Find a loop and face that shares the closest edge
			brep::loop* symmetric_loop = closest_edge->loops().front();
			if (symmetric_loop == closest_loop)
			{
				symmetric_loop = closest_edge->loops().back();
			}
			brep::face* symmetric_face = symmetric_loop->face();
		
			// Find quaternion representing rotation from normal of first face to normal of second face
			const auto& n0 = face_normals[face->index()];
			const auto& n1 = face_normals[symmetric_face->index()];
			const auto rotation = math::rotation(n0, n1);
		
			// Rotate target point
			target_point = rotation * (target_point - closest_point) + closest_point;
		
			// Rotate traversal direction
			traversal_direction = rotation * traversal_direction;
		
			// Move to next face
			face = symmetric_face;
		}
		while (true);
	
		traversal.feature = face;
		traversal.target_point = target_point;
		traversal.closest_point = closest_point;
	
		auto loop_it = face->loops().begin();
		const auto& a = vertex_positions[loop_it->vertex()->index()];
		const auto& b = vertex_positions[(++loop_it)->vertex()->index()];
		const auto& c = vertex_positions[(++loop_it)->vertex()->index()];
		traversal.barycentric = geom::cartesian_to_barycentric(traversal.closest_point, a, b, c);
	
		return traversal;
	}
}

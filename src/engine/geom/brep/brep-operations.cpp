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

#include <engine/geom/brep/brep-operations.hpp>
#include <engine/math/vector.hpp>
#include <algorithm>

namespace geom {

void generate_face_normals(brep_mesh& mesh)
{
	const auto& vertex_positions = mesh.vertices().attributes().at<math::fvec3>("position");
	auto& face_normals =  static_cast<brep_attribute<math::fvec3>&>(*mesh.faces().attributes().try_emplace<math::fvec3>("normal").first);
	
	for (brep_face* face: mesh.faces())
	{
		auto loop = face->loops().begin();
		const auto& a = vertex_positions[loop->vertex()->index()];
		const auto& b = vertex_positions[(++loop)->vertex()->index()];
		const auto& c = vertex_positions[(++loop)->vertex()->index()];
		
		face_normals[face->index()] = math::normalize(math::cross(b - a, c - a));
	}
}

void generate_vertex_normals(brep_mesh& mesh)
{
	const auto& vertex_positions = mesh.vertices().attributes().at<math::fvec3>("position");
	auto& vertex_normals =  static_cast<brep_attribute<math::fvec3>&>(*mesh.vertices().attributes().try_emplace<math::fvec3>("normal").first);
	
	// Init vertex normals to zero
	std::fill(vertex_normals.begin(), vertex_normals.end(), math::fvec3::zero());
	
	if (auto face_normals_it = mesh.faces().attributes().find("normals"); face_normals_it != mesh.faces().attributes().end())
	{
		const auto& face_normals = static_cast<const brep_attribute<math::fvec3>&>(*face_normals_it);
		
		for (brep_face* face: mesh.faces())
		{
			// Get face normal from face normal attribute
			const auto& face_normal = face_normals[face->index()];
			
			// Accumulate vertex normals
			for (brep_loop* loop: face->loops())
			{
				vertex_normals[loop->vertex()->index()] += face_normal;
			}
		}
	}
	else
	{
		for (brep_face* face: mesh.faces())
		{
			// Calculate face normal
			auto loop = face->loops().begin();
			const auto& a = vertex_positions[loop->vertex()->index()];
			const auto& b = vertex_positions[(++loop)->vertex()->index()];
			const auto& c = vertex_positions[(++loop)->vertex()->index()];
			const auto face_normal = math::normalize(math::cross(b - a, c - a));
			
			// Accumulate vertex normals
			for (brep_loop* loop: face->loops())
			{
				vertex_normals[loop->vertex()->index()] += face_normal;
			}
		}
	}
	
	// Normalize vertex normals
	for (auto& vertex_normal: vertex_normals)
	{
		vertex_normal = math::normalize(vertex_normal);
	}
}

void generate_loop_barycentric(brep_mesh& mesh)
{
	const auto& vertex_positions = mesh.vertices().attributes().at<math::fvec3>("position");
	auto& loop_barycentric =  static_cast<brep_attribute<math::fvec3>&>(*mesh.loops().attributes().try_emplace<math::fvec3>("barycentric").first);
	
	for (brep_face* face: mesh.faces())
	{
		auto loop = face->loops().begin();
		loop_barycentric[loop->index()] = {1.0f, 0.0f, 0.0f};
		loop_barycentric[(++loop)->index()] = {0.0f, 1.0f, 0.0f};
		loop_barycentric[(++loop)->index()] = {0.0f, 0.0f, 1.0f};
	}
}

} // namespace geom

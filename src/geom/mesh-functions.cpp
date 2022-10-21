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

#include "mesh-functions.hpp"
#include "utility/fundamental-types.hpp"
#include <unordered_map>

namespace geom {

struct edge_hasher
{
	std::size_t operator()(const std::array<std::size_t, 2>& v) const noexcept
	{
		std::size_t hash = std::hash<std::size_t>()(v[0]);
		return hash ^ (std::hash<std::size_t>()(v[1]) + 0x9e3779b9 + (hash << 6) + (hash >> 2));
	}
};

void create_triangle_mesh(mesh& mesh, const std::vector<float3>& vertices, const std::vector<std::array<std::uint_fast32_t, 3>>& triangles)
{
	for (const auto& vertex: vertices)
		mesh.add_vertex(vertex);

	std::unordered_map<std::array<std::size_t, 2>, geom::mesh::edge*, edge_hasher> edge_map;
	const std::vector<mesh::vertex*>& mesh_vertices = mesh.get_vertices();
	std::vector<geom::mesh::edge*> loop(3);

	for (const auto& triangle: triangles)
	{
		geom::mesh::vertex* triangle_vertices[3] =
		{
			mesh_vertices[triangle[0]],
			mesh_vertices[triangle[1]],
			mesh_vertices[triangle[2]]
		};

		for (int j = 0; j < 3; ++j)
		{
			geom::mesh::vertex* start = triangle_vertices[j];
			geom::mesh::vertex* end = triangle_vertices[(j + 1) % 3];

			if (auto it = edge_map.find({start->index, end->index}); it != edge_map.end())
			{
				loop[j] = it->second;
			}
			else
			{
				loop[j] = mesh.add_edge(start, end);
				edge_map[{start->index, end->index}] = loop[j];
				edge_map[{end->index, start->index}] = loop[j]->symmetric;
			}

		}

		mesh.add_face(loop);
	}
}

void calculate_face_normals(float3* normals, const mesh& mesh)
{
	const std::vector<mesh::face*>& faces = mesh.get_faces();

	for (std::size_t i = 0; i < faces.size(); ++i)
	{
		const mesh::face& face = *(faces[i]);
		const float3& a = face.edge->vertex->position;
		const float3& b = face.edge->next->vertex->position;
		const float3& c = face.edge->previous->vertex->position;

		normals[i] = math::normalize(math::cross(b - a, c - a));
	}
}

float3 calculate_face_normal(const mesh::face& face)
{
	const float3& a = face.edge->vertex->position;
	const float3& b = face.edge->next->vertex->position;
	const float3& c = face.edge->previous->vertex->position;
	return math::normalize(math::cross(b - a, c - a));
}

void calculate_vertex_tangents(float4* tangents, const float2* texcoords, const float3* normals, const mesh& mesh)
{
	const std::vector<mesh::face*>& faces = mesh.get_faces();
	const std::vector<mesh::vertex*>& vertices = mesh.get_vertices();
	
	// Allocate tangent and bitangent buffers
	float3* tangent_buffer = new float3[vertices.size()];
	float3* bitangent_buffer = new float3[vertices.size()];
	for (std::size_t i = 0; i < vertices.size(); ++i)
	{
		tangent_buffer[i] = {0.0f, 0.0f, 0.0f};
		bitangent_buffer[i] = {0.0f, 0.0f, 0.0f};
	}
	
	// Accumulate tangents and bitangents
	for (std::size_t i = 0; i < faces.size(); ++i)
	{
		const mesh::face& face = *(faces[i]);
		std::size_t ia = face.edge->vertex->index;
		std::size_t ib = face.edge->next->vertex->index;
		std::size_t ic = face.edge->previous->vertex->index;
		const float3& a = vertices[ia]->position;
		const float3& b = vertices[ib]->position;
		const float3& c = vertices[ic]->position;
		const float2& uva = texcoords[ia];
		const float2& uvb = texcoords[ib];
		const float2& uvc = texcoords[ic];

		float3 ba = b - a;
		float3 ca = c - a;
		float2 uvba = uvb - uva;
		float2 uvca = uvc - uva;
		
		float f = 1.0f / (uvba.x() * uvca.y() - uvca.x() * uvba.y());
		float3 tangent = (ba * uvca.y() - ca * uvba.y()) * f;
		float3 bitangent = (ba * -uvca.x() + ca * uvba.x()) * f;
		
		tangent_buffer[ia] += tangent;
		tangent_buffer[ib] += tangent;
		tangent_buffer[ic] += tangent;
		bitangent_buffer[ia] += bitangent;
		bitangent_buffer[ib] += bitangent;
		bitangent_buffer[ic] += bitangent;
	}
	
	// Orthogonalize tangents
	for (std::size_t i = 0; i < vertices.size(); ++i)
	{
		const float3& n = normals[i];
		const float3& t = tangent_buffer[i];
		const float3& b = bitangent_buffer[i];
		
		// Gram-Schmidt orthogonalize tangent
		float3 tangent = math::normalize(t - n * math::dot(n, t));
		
		// Calculate bitangent sign
		float bitangent_sign = (math::dot(math::cross(n, t), b) < 0.0f) ? -1.0f : 1.0f;
		
		tangents[i] = {tangent.x(), tangent.y(), tangent.z(), bitangent_sign};
	}
	
	// Free faceted tangents and bitangents
	delete[] tangent_buffer;
	delete[] bitangent_buffer;
}

aabb<float> calculate_bounds(const mesh& mesh)
{
	float3 bounds_min;
	float3 bounds_max;
	for (int i = 0; i < 3; ++i)
	{
		bounds_min[i] = std::numeric_limits<float>::infinity();
		bounds_max[i] = -std::numeric_limits<float>::infinity();
	}

	for (const mesh::vertex* vertex: mesh.get_vertices())
	{
		const auto& position = vertex->position;
		for (int i = 0; i < 3; ++i)
		{
			bounds_min[i] = std::min<float>(bounds_min[i], position[i]);
			bounds_max[i] = std::max<float>(bounds_max[i], position[i]);
		}
	}

	return aabb<float>{bounds_min, bounds_max};
}

mesh::vertex* poke_face(mesh& mesh, std::size_t index)
{
	mesh::face* face = mesh.get_faces()[index];
	
	// Collect face edges and sum edge vertex positions
	mesh::loop loop = {face->edge};
	float3 sum_positions = face->edge->vertex->position;
	for (mesh::edge* edge = face->edge->next; edge != face->edge; edge = edge->next)
	{
		loop.push_back(edge);
		sum_positions += edge->vertex->position;
	}
	
	if (loop.size() <= 2)
		return nullptr;
	
	// Remove face
	mesh.remove_face(face);
	
	// Add vertex in center
	mesh::vertex* center = mesh.add_vertex(sum_positions / static_cast<float>(loop.size()));
	
	// Create first triangle
	geom::mesh::edge* ab = loop[0];
	geom::mesh::edge* bc = mesh.add_edge(ab->next->vertex, center);
	geom::mesh::edge* ca = mesh.add_edge(center, ab->vertex);
	mesh.add_face({ab, bc, ca});
	
	// Save first triangle CA edge
	geom::mesh::edge* first_triangle_ca = ca;
	
	// Create remaining triangles
	for (std::size_t i = 1; i < loop.size(); ++i)
	{
		ab = loop[i];
		ca = bc->symmetric;
		
		if (i == loop.size() - 1)
			bc = first_triangle_ca->symmetric;
		else
			bc = mesh.add_edge(ab->next->vertex, center);
		
		mesh.add_face({ab, bc, ca});
	}
	
	return center;
}

} // namespace geom

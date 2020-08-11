/*
 * Copyright (C) 2020  Christopher J. Howard
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
#include "math/math.hpp"
#include <unordered_map>

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

	std::unordered_map<std::array<std::size_t, 2>, ::mesh::edge*, edge_hasher> edge_map;
	const std::vector<mesh::vertex*>& mesh_vertices = mesh.get_vertices();
	std::vector<::mesh::edge*> loop(3);

	for (const auto& triangle: triangles)
	{
		::mesh::vertex* triangle_vertices[3] =
		{
			mesh_vertices[triangle[0]],
			mesh_vertices[triangle[1]],
			mesh_vertices[triangle[2]]
		};

		for (int j = 0; j < 3; ++j)
		{
			::mesh::vertex* start = triangle_vertices[j];
			::mesh::vertex* end = triangle_vertices[(j + 1) % 3];

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

void calculate_face_normals(float* normals, const mesh& mesh)
{
	const std::vector<mesh::face*>& faces = mesh.get_faces();

	for (std::size_t i = 0; i < faces.size(); ++i)
	{
		const mesh::face& face = *(faces[i]);
		float3& normal = reinterpret_cast<float3&>(normals[i * 3]);

		const float3& a = reinterpret_cast<const float3&>(face.edge->vertex->position);
		const float3& b = reinterpret_cast<const float3&>(face.edge->next->vertex->position);
		const float3& c = reinterpret_cast<const float3&>(face.edge->previous->vertex->position);

		normal = math::normalize(math::cross(b - a, c - a));
	}
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

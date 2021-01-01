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

#include "relief-map.hpp"
#include <array>
#include <cmath>
#include <map>

namespace cart
{

mesh* map_elevation(const std::function<float(float, float)>& function, float scale, std::size_t subdivisions)
{
	// Allocate terrain mesh
	mesh* mesh = new ::mesh();

	// Determine vertex count and placement
	std::size_t columns = static_cast<std::size_t>(std::pow(2, subdivisions));
	std::size_t rows = columns;
	float uv_scale = 1.0f / static_cast<float>(columns);
	//std::size_t vertex_count = (columns + 1) * (rows + 1);
	
	// Generate mesh vertices
	float3 position;
	float2 uv;
	for (std::size_t i = 0; i <= rows; ++i)
	{
		uv.y = static_cast<float>(i) * uv_scale;
		position.z = (uv.y - 0.5f) * scale;
		
		for (std::size_t j = 0; j <= columns; ++j)
		{
			uv.x = static_cast<float>(j) * uv_scale;
			
			position.x = (uv.x - 0.5f) * scale;
			position.y = function(uv.x, uv.y);
			
			mesh->add_vertex(position);
		}
	}
	
	// Function to eliminate duplicate edges
	std::map<std::array<std::size_t, 2>, mesh::edge*> edge_map;
	auto add_or_find_edge = [&](mesh::vertex* start, mesh::vertex* end) -> mesh::edge*
	{
		mesh::edge* edge;
		if (auto it = edge_map.find({start->index, end->index}); it != edge_map.end())
		{
			edge = it->second;
		}
		else
		{
			edge = mesh->add_edge(start, end);
			edge_map[{start->index, end->index}] = edge;
			edge_map[{end->index, start->index}] = edge->symmetric;
		}

		return edge;
	};

	// Connect vertices with edges and faces
	const std::vector<mesh::vertex*>& vertices = mesh->get_vertices();
	for (std::size_t i = 0; i < rows; ++i)
	{
		for (std::size_t j = 0; j < columns; ++j)
		{
			mesh::vertex* a = vertices[i * (columns + 1) + j];
			mesh::vertex* b = vertices[(i + 1) * (columns + 1) + j];
			mesh::vertex* c = vertices[i * (columns + 1) + j + 1];
			mesh::vertex* d = vertices[(i + 1) * (columns + 1) + j + 1];

			// +---+---+
			// | \ | / |
			// |---+---|
			// | / | \ |
			// +---+---+
			if ((j % 2) == (i % 2))
			{
				mesh::edge* ab = add_or_find_edge(a, b);
				mesh::edge* bd = add_or_find_edge(b, d);
				mesh::edge* da = add_or_find_edge(d, a);

				mesh::edge* ca = add_or_find_edge(c, a);
				mesh::edge* ad = da->symmetric;
				mesh::edge* dc = add_or_find_edge(d, c);

				// a---c
				// | \ |
				// b---d
				mesh->add_face({ab, bd, da});
				mesh->add_face({ca, ad, dc});
			}
			else
			{
				mesh::edge* ab = add_or_find_edge(a, b);
				mesh::edge* bc = add_or_find_edge(b, c);
				mesh::edge* ca = add_or_find_edge(c, a);
				mesh::edge* cb = bc->symmetric;
				mesh::edge* bd = add_or_find_edge(b, d);
				mesh::edge* dc = add_or_find_edge(d, c);

				// a---c
				// | / |
				// b---d
				mesh->add_face({ab, bc, ca});
				mesh->add_face({cb, bd, dc});
			}
		}
	}

	return mesh;
}

} // namespace cart

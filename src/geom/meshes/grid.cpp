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

#include "geom/meshes/grid.hpp"
#include <array>
#include <cmath>
#include <map>

namespace geom {
namespace meshes {

geom::mesh* grid_xy(float length, std::size_t subdivisions_x, std::size_t subdivisions_y)
{
	// Allocate new mesh
	geom::mesh* mesh = new geom::mesh();

	// Determine vertex count and placement
	std::size_t columns = subdivisions_x + 1;
	std::size_t rows = subdivisions_y + 1;
	float column_length = length / static_cast<float>(columns);
	float row_length = length / static_cast<float>(rows);
	
	// Generate mesh vertices
	float3 position;
	position.z = 0.0f;
	position.y = -length * 0.5f;
	for (std::size_t i = 0; i <= rows; ++i)
	{
		position.x = -length * 0.5f;
		for (std::size_t j = 0; j <= columns; ++j)
		{
			mesh->add_vertex(position);
			
			position.x += column_length;
		}
		
		position.y += row_length;
	}
	
	// Function to eliminate duplicate edges
	std::map<std::array<std::size_t, 2>, geom::mesh::edge*> edge_map;
	auto add_or_find_edge = [&](geom::mesh::vertex* start, geom::mesh::vertex* end) -> geom::mesh::edge*
	{
		geom::mesh::edge* edge;
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
	const std::vector<geom::mesh::vertex*>& vertices = mesh->get_vertices();
	for (std::size_t i = 0; i < rows; ++i)
	{
		for (std::size_t j = 0; j < columns; ++j)
		{
			geom::mesh::vertex* a = vertices[i * (columns + 1) + j];
			geom::mesh::vertex* b = vertices[(i + 1) * (columns + 1) + j];
			geom::mesh::vertex* c = vertices[i * (columns + 1) + j + 1];
			geom::mesh::vertex* d = vertices[(i + 1) * (columns + 1) + j + 1];
			
			// a---c
			// |   |
			// b---d
			geom::mesh::edge* ab = add_or_find_edge(a, b);
			geom::mesh::edge* bd = add_or_find_edge(b, d);
			geom::mesh::edge* dc = add_or_find_edge(d, c);
			geom::mesh::edge* ca = add_or_find_edge(c, a);
			
			mesh->add_face({ab, bd, dc, ca});
		}
	}

	return mesh;
}

} // namespace meshes
} // namespace geom

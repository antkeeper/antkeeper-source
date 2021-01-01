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

#include "resources/resource-loader.hpp"
#include "geometry/mesh.hpp"
#include "geometry/mesh-functions.hpp"
#include "utility/fundamental-types.hpp"
#include <sstream>
#include <stdexcept>
#include <physfs.h>

template <>
mesh* resource_loader<mesh>::load(resource_manager* resource_manager, PHYSFS_File* file)
{
	std::string line;
	std::vector<float3> vertices;
	std::vector<std::array<std::uint_fast32_t, 3>> triangles;

	while (!PHYSFS_eof(file))
	{
		// Read line
		physfs_getline(file, line);
		
		// Tokenize line
		std::vector<std::string> tokens;
		std::string token;
		std::istringstream linestream(line);
		while (linestream >> token)
			tokens.push_back(token);
		
		// Skip empty lines and comments
		if (tokens.empty() || tokens[0][0] == '#')
			continue;
		
		if (tokens[0] == "v")
		{
			if (tokens.size() != 4)
			{
				std::stringstream stream;
				stream << "resource_loader<mesh>::load(): Invalid line \"" << line << "\"" << std::endl;
				throw std::runtime_error(stream.str());
			}
			
			float3 vertex;
			
			std::stringstream(tokens[1]) >> vertex[0];
			std::stringstream(tokens[2]) >> vertex[1];
			std::stringstream(tokens[3]) >> vertex[2];

			vertices.push_back(vertex);
		}
		else if (tokens[0] == "f")
		{
			if (tokens.size() != 4)
			{
				std::stringstream stream;
				stream << "resource_loader<mesh>::load(): Invalid line \"" << line << "\"" << std::endl;
				throw std::runtime_error(stream.str());

			}
			
			std::uint_fast32_t a, b, c;
			std::stringstream(tokens[1]) >> a;
			std::stringstream(tokens[2]) >> b;
			std::stringstream(tokens[3]) >> c;
			triangles.push_back({a - 1, b - 1, c - 1});
		}
	}

	mesh* mesh = new ::mesh();
	create_triangle_mesh(*mesh, vertices, triangles);

	return mesh;
}


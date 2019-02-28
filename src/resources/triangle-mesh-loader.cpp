/*
 * Copyright (C) 2017-2019  Christopher J. Howard
 *
 * This file is part of Antkeeper Source Code.
 *
 * Antkeeper Source Code is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Antkeeper Source Code is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Antkeeper Source Code.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "resource-loader.hpp"
#include <sstream>
#include <stdexcept>

#include <emergent/emergent.hpp>
using namespace Emergent;

template <>
TriangleMesh* ResourceLoader<TriangleMesh>::load(ResourceManager* resourceManager, std::istream* is)
{
	std::string line;
	std::vector<Vector3> vertices;
	std::vector<std::size_t> indices;

	while (is->good() && std::getline(*is, line))
	{
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
				stream << "ResourceLoader<TriangleMesh>::load(): Invalid line \"" << line << "\"" << std::endl;
				throw std::runtime_error(stream.str());
			}
			
			Vector3 vertex;
			
			std::stringstream(tokens[1]) >> vertex.x;
			std::stringstream(tokens[2]) >> vertex.y;
			std::stringstream(tokens[3]) >> vertex.z;

			vertices.push_back(vertex);
		}
		else if (tokens[0] == "f")
		{
			if (tokens.size() != 4)
			{
				std::stringstream stream;
				stream << "ResourceLoader<TriangleMesh>::load(): Invalid line \"" << line << "\"" << std::endl;
				throw std::runtime_error(stream.str());

			}
			
			std::size_t a, b, c;
			std::stringstream(tokens[1]) >> a;
			std::stringstream(tokens[2]) >> b;
			std::stringstream(tokens[3]) >> c;
			
			a -= 1;
			b -= 1;
			c -= 1;
			
			indices.push_back(a);
			indices.push_back(b);
			indices.push_back(c);
		}
	}

	return new TriangleMesh(vertices, indices);
}


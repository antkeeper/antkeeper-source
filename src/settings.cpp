/*
 * Copyright (C) 2017  Christopher J. Howard
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

#include "settings.hpp"
#include <fstream>
#include <sstream>
#include <vector>
#include <iostream>

bool ParameterDict::load(const std::string& filename)
{
	// Open parameter dict file
	std::ifstream file(filename.c_str());
	if (!file.is_open())
	{
		std::cerr << "Failed to open file \"" << filename << "\"" << std::endl;
		return false;
	}
	
	// Read file
	std::string line;
	while (file.good() && std::getline(file, line))
	{
		// Tokenize line (tab-delimeted)
		std::vector<std::string> tokens;
		std::string token;
		std::istringstream linestream(line);
		while (std::getline(linestream, token, '\t'))
			tokens.push_back(token);
		
		if (tokens.empty() || tokens[0][0] == '#')
			continue;
		
		if (tokens.size() != 2)
		{
			std::cerr << "Invalid line \"" << line << "\" in file \"" << filename << "\"" << std::endl;
			continue;
		}
		
		parameters[tokens[0]] = tokens[1];
	}
	
	file.close();
	
	return true;
}

bool ParameterDict::save(const std::string& filename)
{
	std::ofstream file(filename.c_str());
	if (!file.is_open())
	{
		std::cerr << "Failed to open file \"" << filename << "\"" << std::endl;
		return false;
	}
	
	for (auto it = parameters.begin(); it != parameters.end(); ++it)
		file << it->first << "\t" << it->second << std::endl;
	
	file.close();
	
	return true;
}

void ParameterDict::clear()
{
	parameters.clear();
}

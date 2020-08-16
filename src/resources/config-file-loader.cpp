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

#include "resources/resource-loader.hpp"
#include "resources/resource-manager.hpp"
#include "resources/config-file.hpp"
#include "resources/text-file.hpp"

template <>
config_file* resource_loader<config_file>::load(resource_manager* resource_manager, PHYSFS_File* file)
{
	// Load as text file
	text_file* text = resource_loader<text_file>::load(resource_manager, file);
	
	config_file* config = new config_file();
	for (const std::string& line: *text)
	{
		if (line[0] == '#')
			continue;
		
		std::size_t delimeter = line.find_first_of('=');
		if (delimeter == std::string::npos)
			continue;
		
		std::string name = line.substr(0, delimeter);
		std::string value = line.substr(delimeter + 1, line.length() - delimeter - 1);
		
		if (name.empty() || value.empty())
			continue;
		
		config->set<std::string>(name, value);
	}
	
	delete text;
	
	return config;
}

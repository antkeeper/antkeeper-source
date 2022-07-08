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
#include "resources/resource-manager.hpp"
#include "resources/json.hpp"
#include "game/ant/trait/pilosity.hpp"
#include <stdexcept>

using namespace game::ant;

template <>
trait::pilosity* resource_loader<trait::pilosity>::load(resource_manager* resource_manager, PHYSFS_File* file, const std::filesystem::path& path)
{
	// Load JSON data
	json* data = resource_loader<json>::load(resource_manager, file, path);
	
	// Validate trait file
	auto pilosity_element = data->find("pilosity");
	if (pilosity_element == data->end())
		throw std::runtime_error("Invalid pilosity trait.");
	
	// Allocate pilosity trait
	trait::pilosity* pilosity = new trait::pilosity();
	
	// Parse pilosity density
	pilosity->density = 0.0f;
	if (auto density_element = pilosity_element->find("density"); density_element != pilosity_element->end())
		pilosity->density = density_element->get<float>();
	
	// Free JSON data
	delete data;
	
	return pilosity;
}

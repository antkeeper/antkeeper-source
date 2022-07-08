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
#include "game/ant/trait/larva.hpp"
#include "render/model.hpp"
#include <stdexcept>

using namespace game::ant;

template <>
trait::larva* resource_loader<trait::larva>::load(resource_manager* resource_manager, PHYSFS_File* file, const std::filesystem::path& path)
{
	// Load JSON data
	json* data = resource_loader<json>::load(resource_manager, file, path);
	
	// Validate trait file
	auto larva_element = data->find("larva");
	if (larva_element == data->end())
		throw std::runtime_error("Invalid larva trait.");
	
	// Allocate larva trait
	trait::larva* larva = new trait::larva();
	
	// Load larva model
	auto model_element = larva_element->find("model");
	if (model_element == larva_element->end())
		throw std::runtime_error("Larva trait doesn't specify larva model.");
	larva->model = resource_manager->load<render::model>(model_element->get<std::string>());
	
	// Parse larva instars
	larva->instars = 0;
	if (auto instars_element = larva_element->find("instars"); instars_element != larva_element->end())
		larva->instars = instars_element->get<int>();
	
	// Free JSON data
	delete data;
	
	return larva;
}

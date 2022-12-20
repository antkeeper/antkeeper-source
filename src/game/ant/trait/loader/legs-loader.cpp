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
#include "game/ant/trait/legs.hpp"
#include "render/model.hpp"
#include <stdexcept>

using namespace game::ant;

template <>
trait::legs* resource_loader<trait::legs>::load(resource_manager* resource_manager, PHYSFS_File* file, const std::filesystem::path& path)
{
	// Load JSON data
	json* data = resource_loader<json>::load(resource_manager, file, path);
	
	// Validate trait file
	auto legs_element = data->find("legs");
	if (legs_element == data->end())
		throw std::runtime_error("Invalid legs trait.");
	
	// Allocate and init legs trait
	trait::legs* legs = new trait::legs();
	legs->model = nullptr;
	legs->speed = 1.0f;
	legs->grip = 1.0f;
	
	// Load model
	auto model_element = legs_element->find("model");
	if (model_element == legs_element->end())
		throw std::runtime_error("Legs trait doesn't specify legs model.");
	legs->model = resource_manager->load<render::model>(model_element->get<std::string>());
	
	// Parse speed
	if (auto element = legs_element->find("speed"); element != legs_element->end())
		legs->speed = element->get<float>();
	
	// Parse grip
	if (auto element = legs_element->find("grip"); element != legs_element->end())
		legs->grip = element->get<float>();
	
	// Free JSON data
	delete data;
	
	return legs;
}

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
#include "game/ant/trait/egg.hpp"
#include "render/model.hpp"
#include <stdexcept>

using namespace game::ant;

template <>
trait::egg* resource_loader<trait::egg>::load(resource_manager* resource_manager, PHYSFS_File* file, const std::filesystem::path& path)
{
	// Load JSON data
	json* data = resource_loader<json>::load(resource_manager, file, path);
	
	// Validate trait file
	auto egg_element = data->find("egg");
	if (egg_element == data->end())
		throw std::runtime_error("Invalid egg trait.");
	
	// Allocate egg trait
	trait::egg* egg = new trait::egg();
	
	// Load egg model
	auto model_element = egg_element->find("model");
	if (model_element == egg_element->end())
		throw std::runtime_error("Egg trait doesn't specify egg model.");
	egg->model = resource_manager->load<render::model>(model_element->get<std::string>());
	
	// Free JSON data
	delete data;
	
	return egg;
}

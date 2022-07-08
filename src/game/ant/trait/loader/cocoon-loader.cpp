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
#include "game/ant/trait/cocoon.hpp"
#include "render/model.hpp"
#include <stdexcept>

using namespace game::ant;

template <>
trait::cocoon* resource_loader<trait::cocoon>::load(resource_manager* resource_manager, PHYSFS_File* file, const std::filesystem::path& path)
{
	// Load JSON data
	json* data = resource_loader<json>::load(resource_manager, file, path);
	
	// Validate trait file
	auto cocoon_element = data->find("cocoon");
	if (cocoon_element == data->end())
		throw std::runtime_error("Invalid cocoon trait.");
	
	// Allocate cocoon trait
	trait::cocoon* cocoon = new trait::cocoon();
	
	// Parse cocoon present
	cocoon->present = false;
	if (auto present_element = cocoon_element->find("present"); present_element != cocoon_element->end())
		cocoon->present = present_element->get<bool>();
	
	// Load cocoon model (if present)
	if (cocoon->present)
	{
		auto model_element = cocoon_element->find("model");
		if (model_element == cocoon_element->end())
			throw std::runtime_error("Cocoon trait doesn't specify cocoon model.");
		cocoon->model = resource_manager->load<render::model>(model_element->get<std::string>());
	}
	else
	{
		cocoon->model = nullptr;
	}
	
	// Free JSON data
	delete data;
	
	return cocoon;
}

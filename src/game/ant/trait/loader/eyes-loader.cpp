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
#include "game/ant/trait/eyes.hpp"
#include "render/model.hpp"
#include <stdexcept>

using namespace game::ant;

template <>
trait::eyes* resource_loader<trait::eyes>::load(resource_manager* resource_manager, PHYSFS_File* file, const std::filesystem::path& path)
{
	// Load JSON data
	json* data = resource_loader<json>::load(resource_manager, file, path);
	
	// Validate trait file
	auto eyes_element = data->find("eyes");
	if (eyes_element == data->end())
		throw std::runtime_error("Invalid eyes trait.");
	
	// Allocate and init eyes trait
	trait::eyes* eyes = new trait::eyes();
	eyes->present = false;
	eyes->model = nullptr;
	eyes->length = 0.0f;
	eyes->width = 0.0f;
	eyes->height = 0.0f;
	eyes->ommatidia_count = 0;
	
	// Parse eyes present
	if (auto present_element = eyes_element->find("present"); present_element != eyes_element->end())
		eyes->present = present_element->get<bool>();
	
	if (eyes->present)
	{
		// Load eyes model
		auto model_element = eyes_element->find("model");
		if (model_element == eyes_element->end())
			throw std::runtime_error("Eyes trait doesn't specify eyes model.");
		eyes->model = resource_manager->load<render::model>(model_element->get<std::string>());
		
		// Parse length
		if (auto element = eyes_element->find("length"); element != eyes_element->end())
			eyes->length = element->get<float>();
		
		// Parse width
		if (auto element = eyes_element->find("width"); element != eyes_element->end())
			eyes->width = element->get<float>();
		
		// Parse height
		if (auto element = eyes_element->find("height"); element != eyes_element->end())
			eyes->height = element->get<float>();
		
		// Parse ommatidia count
		if (auto element = eyes_element->find("ommatidia_count"); element != eyes_element->end())
			eyes->ommatidia_count = element->get<int>();
	}
	
	// Free JSON data
	delete data;
	
	return eyes;
}

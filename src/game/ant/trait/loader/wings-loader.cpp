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
#include "game/ant/trait/wings.hpp"
#include "render/model.hpp"
#include <stdexcept>

using namespace game::ant;

template <>
trait::wings* resource_loader<trait::wings>::load(resource_manager* resource_manager, PHYSFS_File* file, const std::filesystem::path& path)
{
	// Load JSON data
	json* data = resource_loader<json>::load(resource_manager, file, path);
	
	// Validate trait file
	auto wings_element = data->find("wings");
	if (wings_element == data->end())
		throw std::runtime_error("Invalid wings trait.");
	
	// Allocate and init wings trait
	trait::wings* wings = new trait::wings();
	wings->forewings_model = nullptr;
	wings->hindwings_model = nullptr;
	wings->present = false;
	wings->forewing_length = 0.0f;
	wings->forewing_width = 0.0f;
	wings->forewing_venation = 0.0f;
	wings->hindwing_length = 0.0f;
	wings->hindwing_width = 0.0f;
	wings->hindwing_venation = 0.0f;
	
	// Parse wings present
	if (auto present_element = wings_element->find("present"); present_element != wings_element->end())
		wings->present = present_element->get<bool>();
	
	if (wings->present)
	{
		// Load forewings model
		auto forewings_model_element = wings_element->find("forewings_model");
		if (forewings_model_element == wings_element->end())
			throw std::runtime_error("Wings trait doesn't specify forewings model.");
		wings->forewings_model = resource_manager->load<render::model>(forewings_model_element->get<std::string>());
		
		// Load hindwings model
		auto hindwings_model_element = wings_element->find("hindwings_model");
		if (hindwings_model_element == wings_element->end())
			throw std::runtime_error("Wings trait doesn't specify hindwings model.");
		wings->hindwings_model = resource_manager->load<render::model>(hindwings_model_element->get<std::string>());
		
		// Parse forewing length
		if (auto element = wings_element->find("forewing_length"); element != wings_element->end())
			wings->forewing_length = element->get<float>();
		
		// Parse forewing width
		if (auto element = wings_element->find("forewing_width"); element != wings_element->end())
			wings->forewing_width = element->get<float>();
		
		// Parse forewing venation
		if (auto element = wings_element->find("forewing_venation"); element != wings_element->end())
			wings->forewing_venation = element->get<float>();
		
		// Parse hindwing length
		if (auto element = wings_element->find("hindwing_length"); element != wings_element->end())
			wings->hindwing_length = element->get<float>();
		
		// Parse hindwing width
		if (auto element = wings_element->find("hindwing_width"); element != wings_element->end())
			wings->hindwing_width = element->get<float>();
		
		// Parse hindwing venation
		if (auto element = wings_element->find("hindwing_venation"); element != wings_element->end())
			wings->hindwing_venation = element->get<float>();
	}
	
	// Free JSON data
	delete data;
	
	return wings;
}

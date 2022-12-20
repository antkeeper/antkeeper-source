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
#include "game/ant/trait/ocelli.hpp"
#include "render/model.hpp"
#include <stdexcept>

using namespace game::ant;

template <>
trait::ocelli* resource_loader<trait::ocelli>::load(resource_manager* resource_manager, PHYSFS_File* file, const std::filesystem::path& path)
{
	// Load JSON data
	json* data = resource_loader<json>::load(resource_manager, file, path);
	
	// Validate trait file
	auto ocelli_element = data->find("ocelli");
	if (ocelli_element == data->end())
		throw std::runtime_error("Invalid ocelli trait.");
	
	// Allocate and init ocelli trait
	trait::ocelli* ocelli = new trait::ocelli();
	ocelli->lateral_ocelli_model = nullptr;
	ocelli->median_ocellus_model = nullptr;
	ocelli->lateral_ocelli_present = false;
	ocelli->median_ocellus_present = false;
	ocelli->width = 0.0f;
	ocelli->height = 0.0f;
	
	// Parse lateral ocelli present
	if (auto element = ocelli_element->find("lateral_ocelli_present"); element != ocelli_element->end())
		ocelli->lateral_ocelli_present = element->get<bool>();
	
	// Parse median ocellus present
	if (auto element = ocelli_element->find("median_ocellus_present"); element != ocelli_element->end())
		ocelli->median_ocellus_present = element->get<bool>();
	
	// Parse width
	if (auto element = ocelli_element->find("width"); element != ocelli_element->end())
		ocelli->width = element->get<float>();
	
	// Parse height
	if (auto element = ocelli_element->find("height"); element != ocelli_element->end())
		ocelli->height = element->get<float>();

	// Load lateral ocelli model, if present
	if (ocelli->lateral_ocelli_present)
	{
		auto lateral_ocelli_model_element = ocelli_element->find("lateral_ocelli_model");
		if (lateral_ocelli_model_element == ocelli_element->end() || lateral_ocelli_model_element->is_null())
			throw std::runtime_error("Ocelli trait doesn't specify lateral ocelli model.");
		
		ocelli->lateral_ocelli_model = resource_manager->load<render::model>(lateral_ocelli_model_element->get<std::string>());
	}
	
	// Load median ocellus model, if present
	if (ocelli->median_ocellus_present)
	{
		auto median_ocellus_model_element = ocelli_element->find("median_ocellus_model");
		if (median_ocellus_model_element == ocelli_element->end() || median_ocellus_model_element->is_null())
			throw std::runtime_error("Ocelli trait doesn't specify median ocellus model.");
		
		ocelli->median_ocellus_model = resource_manager->load<render::model>(median_ocellus_model_element->get<std::string>());
	}
	
	// Free JSON data
	delete data;
	
	return ocelli;
}

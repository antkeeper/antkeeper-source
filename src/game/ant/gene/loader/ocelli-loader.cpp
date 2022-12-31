/*
 * Copyright (C) 2023  Christopher J. Howard
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

#include "game/ant/gene/loader/gene-loader.hpp"
#include "resources/resource-loader.hpp"
#include "resources/resource-manager.hpp"
#include "resources/json.hpp"
#include "game/ant/gene/ocelli.hpp"
#include "render/model.hpp"
#include <stdexcept>

using namespace game::ant;

static void deserialize_ocelli_phene(phene::ocelli& phene, const json& phene_element, resource_manager* resource_manager)
{
	phene.lateral_ocelli_present = false;
	phene.median_ocellus_present = false;
	phene.lateral_ocelli_model = nullptr;
	phene.median_ocellus_model = nullptr;
	phene.width = 0.0f;
	phene.height = 0.0f;
	
	// Parse lateral ocelli present
	if (auto element = phene_element.find("lateral_ocelli_present"); element != phene_element.end())
		phene.lateral_ocelli_present = element->get<bool>();
	
	// Parse median ocelli present
	if (auto element = phene_element.find("median_ocellus_present"); element != phene_element.end())
		phene.median_ocellus_present = element->get<bool>();
	
	// Parse width
	if (auto element = phene_element.find("width"); element != phene_element.end())
		phene.width = element->get<float>();
	
	// Parse height
	if (auto element = phene_element.find("height"); element != phene_element.end())
		phene.height = element->get<float>();
	
	// Load lateral ocelli model, if present
	if (phene.lateral_ocelli_present)
	{
		if (auto element = phene_element.find("lateral_ocelli_model"); element != phene_element.end())
			phene.lateral_ocelli_model = resource_manager->load<render::model>(element->get<std::string>());
	}
	
	// Load median ocellus model, if present
	if (phene.median_ocellus_present)
	{
		if (auto element = phene_element.find("median_ocellus_model"); element != phene_element.end())
			phene.median_ocellus_model = resource_manager->load<render::model>(element->get<std::string>());
	}
}

template <>
gene::ocelli* resource_loader<gene::ocelli>::load(resource_manager* resource_manager, PHYSFS_File* file, const std::filesystem::path& path)
{
	// Load JSON data
	json* data = resource_loader<json>::load(resource_manager, file, path);
	
	// Validate gene file
	auto ocelli_element = data->find("ocelli");
	if (ocelli_element == data->end())
		throw std::runtime_error("Invalid ocelli gene.");
	
	// Allocate gene
	gene::ocelli* ocelli = new gene::ocelli();
	
	// Deserialize gene
	gene::deserialize_gene(*ocelli, &deserialize_ocelli_phene, *ocelli_element, resource_manager);
	
	// Free JSON data
	delete data;
	
	return ocelli;
}

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
#include <engine/resources/resource-loader.hpp>
#include <engine/resources/resource-manager.hpp>
#include <engine/resources/json.hpp>
#include "game/ant/gene/wings.hpp"
#include <engine/render/model.hpp>
#include <stdexcept>

using namespace ::ant;

static void deserialize_wings_phene(phene::wings& phene, const json& phene_element, resource_manager* resource_manager)
{
	phene.present = false;
	phene.forewings_model = nullptr;
	phene.hindwings_model = nullptr;
	phene.forewing_length = 0.0f;
	phene.forewing_width = 0.0f;
	phene.forewing_venation = 0.0f;
	phene.hindwing_length = 0.0f;
	phene.hindwing_width = 0.0f;
	phene.hindwing_venation = 0.0f;
	
	// Parse present
	if (auto element = phene_element.find("present"); element != phene_element.end())
		phene.present = element->get<bool>();
	
	if (phene.present)
	{
		// Load forewings model
		if (auto element = phene_element.find("forewings_model"); element != phene_element.end())
			phene.forewings_model = resource_manager->load<render::model>(element->get<std::string>());
		
		// Load hindwings model
		if (auto element = phene_element.find("hindwings_model"); element != phene_element.end())
			phene.hindwings_model = resource_manager->load<render::model>(element->get<std::string>());
		
		// Parse forewing length
		if (auto element = phene_element.find("forewing_length"); element != phene_element.end())
			phene.forewing_length = element->get<float>();
		
		// Parse forewing width
		if (auto element = phene_element.find("forewing_width"); element != phene_element.end())
			phene.forewing_width = element->get<float>();
		
		// Parse forewing venation
		if (auto element = phene_element.find("forewing_venation"); element != phene_element.end())
			phene.forewing_venation = element->get<float>();
		
		// Parse hindwing length
		if (auto element = phene_element.find("hindwing_length"); element != phene_element.end())
			phene.hindwing_length = element->get<float>();
		
		// Parse hindwing width
		if (auto element = phene_element.find("hindwing_width"); element != phene_element.end())
			phene.hindwing_width = element->get<float>();
		
		// Parse hindwing venation
		if (auto element = phene_element.find("hindwing_venation"); element != phene_element.end())
			phene.hindwing_venation = element->get<float>();
	}
}

template <>
gene::wings* resource_loader<gene::wings>::load(resource_manager* resource_manager, PHYSFS_File* file, const std::filesystem::path& path)
{
	// Load JSON data
	json* data = resource_loader<json>::load(resource_manager, file, path);
	
	// Validate gene file
	auto wings_element = data->find("wings");
	if (wings_element == data->end())
		throw std::runtime_error("Invalid wings gene.");
	
	// Allocate gene
	gene::wings* wings = new gene::wings();
	
	// Deserialize gene
	gene::deserialize_gene(*wings, &deserialize_wings_phene, *wings_element, resource_manager);
	
	// Free JSON data
	delete data;
	
	return wings;
}

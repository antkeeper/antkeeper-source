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
#include "game/ant/gene/eyes.hpp"
#include "render/model.hpp"
#include <stdexcept>

using namespace game::ant;

static void deserialize_eyes_phene(phene::eyes& phene, const json& phene_element, resource_manager* resource_manager)
{
	phene.present = false;
	phene.model = nullptr;
	phene.length = 0.0f;
	phene.width = 0.0f;
	phene.height = 0.0f;
	phene.ommatidia_count = 0;
	
	// Parse present
	if (auto element = phene_element.find("present"); element != phene_element.end())
		phene.present = element->get<bool>();
	
	if (phene.present)
	{
		// Load eyes model
		if (auto element = phene_element.find("model"); element != phene_element.end())
			phene.model = resource_manager->load<render::model>(element->get<std::string>());
		
		// Parse length
		if (auto element = phene_element.find("length"); element != phene_element.end())
			phene.length = element->get<float>();
		
		// Parse width
		if (auto element = phene_element.find("width"); element != phene_element.end())
			phene.width = element->get<float>();
		
		// Parse height
		if (auto element = phene_element.find("height"); element != phene_element.end())
			phene.height = element->get<float>();
		
		// Parse ommatidia count
		if (auto element = phene_element.find("ommatidia_count"); element != phene_element.end())
			phene.ommatidia_count = element->get<int>();
	}
}

template <>
gene::eyes* resource_loader<gene::eyes>::load(resource_manager* resource_manager, PHYSFS_File* file, const std::filesystem::path& path)
{
	// Load JSON data
	json* data = resource_loader<json>::load(resource_manager, file, path);
	
	// Validate gene file
	auto eyes_element = data->find("eyes");
	if (eyes_element == data->end())
		throw std::runtime_error("Invalid eyes gene.");
	
	// Allocate gene
	gene::eyes* eyes = new gene::eyes();
	
	// Deserialize gene
	gene::deserialize_gene(*eyes, &deserialize_eyes_phene, *eyes_element, resource_manager);
	
	// Free JSON data
	delete data;
	
	return eyes;
}

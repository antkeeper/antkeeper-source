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

#include "game/ant/gene/loader/gene-loader.hpp"
#include "resources/resource-loader.hpp"
#include "resources/resource-manager.hpp"
#include "resources/json.hpp"
#include "game/ant/gene/sting.hpp"
#include "render/model.hpp"
#include <stdexcept>

using namespace game::ant;

static void deserialize_sting_phene(phene::sting& phene, const json& phene_element, resource_manager* resource_manager)
{
	phene.present = false;
	phene.model = nullptr;
	
	// Parse present
	if (auto element = phene_element.find("present"); element != phene_element.end())
		phene.present = element->get<bool>();
	
	if (phene.present)
	{
		// Load sting model
		if (auto element = phene_element.find("model"); element != phene_element.end())
			phene.model = resource_manager->load<render::model>(element->get<std::string>());
	}
}

template <>
gene::sting* resource_loader<gene::sting>::load(resource_manager* resource_manager, PHYSFS_File* file, const std::filesystem::path& path)
{
	// Load JSON data
	json* data = resource_loader<json>::load(resource_manager, file, path);
	
	// Validate gene file
	auto sting_element = data->find("sting");
	if (sting_element == data->end())
		throw std::runtime_error("Invalid sting gene.");
	
	// Allocate gene
	gene::sting* sting = new gene::sting();
	
	// Deserialize gene
	gene::deserialize_gene(*sting, &deserialize_sting_phene, *sting_element, resource_manager);
	
	// Free JSON data
	delete data;
	
	return sting;
}

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
#include "game/ant/gene/waist.hpp"
#include <engine/render/model.hpp>
#include <stdexcept>

using namespace ::ant;

static void deserialize_waist_phene(phene::waist& phene, const json& phene_element, resource_manager* resource_manager)
{
	phene.model = nullptr;
	phene.petiole_present = false;
	phene.petiole_length = 0.0f;
	phene.petiole_width = 0.0f;
	phene.petiole_height = 0.0f;
	phene.petiole_spinescence = 0.0f;
	phene.postpetiole_present = false;
	phene.postpetiole_length = 0.0f;
	phene.postpetiole_width = 0.0f;
	phene.postpetiole_height = 0.0f;
	phene.postpetiole_spinescence = 0.0f;
	
	// Load waist model
	if (auto element = phene_element.find("model"); element != phene_element.end())
		phene.model = resource_manager->load<render::model>(element->get<std::string>());
	
	// Parse petiole present
	if (auto element = phene_element.find("petiole_present"); element != phene_element.end())
		phene.petiole_present = element->get<bool>();
	
	// Parse postpetiole present
	if (auto element = phene_element.find("postpetiole_present"); element != phene_element.end())
		phene.postpetiole_present = element->get<bool>();
	
	if (phene.petiole_present)
	{
		// Parse petiole length
		if (auto element = phene_element.find("petiole_length"); element != phene_element.end())
			phene.petiole_length = element->get<float>();
		
		// Parse petiole width
		if (auto element = phene_element.find("petiole_width"); element != phene_element.end())
			phene.petiole_width = element->get<float>();
		
		// Parse petiole height
		if (auto element = phene_element.find("petiole_height"); element != phene_element.end())
			phene.petiole_height = element->get<float>();
		
		// Parse petiole spinescence
		if (auto element = phene_element.find("petiole_spinescence"); element != phene_element.end())
			phene.petiole_spinescence = element->get<float>();
	}
	
	if (phene.postpetiole_present)
	{
		// Parse postpetiole length
		if (auto element = phene_element.find("postpetiole_length"); element != phene_element.end())
			phene.postpetiole_length = element->get<float>();
		
		// Parse postpetiole width
		if (auto element = phene_element.find("postpetiole_width"); element != phene_element.end())
			phene.postpetiole_width = element->get<float>();
		
		// Parse postpetiole height
		if (auto element = phene_element.find("postpetiole_height"); element != phene_element.end())
			phene.postpetiole_height = element->get<float>();
		
		// Parse postpetiole spinescence
		if (auto element = phene_element.find("postpetiole_spinescence"); element != phene_element.end())
			phene.postpetiole_spinescence = element->get<float>();
	}
}

template <>
gene::waist* resource_loader<gene::waist>::load(resource_manager* resource_manager, PHYSFS_File* file, const std::filesystem::path& path)
{
	// Load JSON data
	json* data = resource_loader<json>::load(resource_manager, file, path);
	
	// Validate gene file
	auto waist_element = data->find("waist");
	if (waist_element == data->end())
		throw std::runtime_error("Invalid waist gene.");
	
	// Allocate gene
	gene::waist* waist = new gene::waist();
	
	// Deserialize gene
	gene::deserialize_gene(*waist, &deserialize_waist_phene, *waist_element, resource_manager);
	
	// Free JSON data
	delete data;
	
	return waist;
}

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
#include "game/ant/gene/head.hpp"
#include "render/model.hpp"
#include <stdexcept>

using namespace game::ant;

static void deserialize_head_phene(phene::head& phene, const json& phene_element, resource_manager* resource_manager)
{
	phene.model = nullptr;
	phene.length = 0.0f;
	phene.width = 0.0f;
	phene.phragmosis = 0.0f;
	
	// Load head model
	if (auto element = phene_element.find("model"); element != phene_element.end())
		phene.model = resource_manager->load<render::model>(element->get<std::string>());
	
	// Parse length
	if (auto element = phene_element.find("length"); element != phene_element.end())
		phene.length = element->get<float>();
	
	// Parse width
	if (auto element = phene_element.find("width"); element != phene_element.end())
		phene.width = element->get<float>();
	
	// Parse phragmosis
	if (auto element = phene_element.find("phragmosis"); element != phene_element.end())
		phene.phragmosis = element->get<float>();
}

template <>
gene::head* resource_loader<gene::head>::load(resource_manager* resource_manager, PHYSFS_File* file, const std::filesystem::path& path)
{
	// Load JSON data
	json* data = resource_loader<json>::load(resource_manager, file, path);
	
	// Validate gene file
	auto head_element = data->find("head");
	if (head_element == data->end())
		throw std::runtime_error("Invalid head gene.");
	
	// Allocate gene
	gene::head* head = new gene::head();
	
	// Deserialize gene
	gene::deserialize_gene(*head, &deserialize_head_phene, *head_element, resource_manager);
	
	// Free JSON data
	delete data;
	
	return head;
}

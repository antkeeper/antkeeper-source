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
#include "game/ant/gene/sculpturing.hpp"
#include "gl/texture-2d.hpp"
#include <stdexcept>

using namespace game::ant;

static void deserialize_sculpturing_phene(phene::sculpturing& phene, const json& phene_element, resource_manager* resource_manager)
{
	phene.normal_map = nullptr;
	phene.roughness = 0.0f;
	
	// Load normal map
	if (auto element = phene_element.find("normal_map"); element != phene_element.end())
		phene.normal_map = resource_manager->load<gl::texture_2d>(element->get<std::string>());
	
	// Parse roughness
	if (auto element = phene_element.find("roughness"); element != phene_element.end())
		phene.roughness = element->get<float>();
}

template <>
gene::sculpturing* resource_loader<gene::sculpturing>::load(resource_manager* resource_manager, PHYSFS_File* file, const std::filesystem::path& path)
{
	// Load JSON data
	json* data = resource_loader<json>::load(resource_manager, file, path);
	
	// Validate gene file
	auto sculpturing_element = data->find("sculpturing");
	if (sculpturing_element == data->end())
		throw std::runtime_error("Invalid sculpturing gene.");
	
	// Allocate gene
	gene::sculpturing* sculpturing = new gene::sculpturing();
	
	// Deserialize gene
	gene::deserialize_gene(*sculpturing, &deserialize_sculpturing_phene, *sculpturing_element, resource_manager);
	
	// Free JSON data
	delete data;
	
	return sculpturing;
}

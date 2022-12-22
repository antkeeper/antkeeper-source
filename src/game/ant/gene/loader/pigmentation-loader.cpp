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
#include "game/ant/gene/pigmentation.hpp"
#include "render/material.hpp"
#include <stdexcept>

using namespace game::ant;

static void deserialize_pigmentation_phene(phene::pigmentation& phene, const json& phene_element, resource_manager* resource_manager)
{
	phene.material = nullptr;
	
	// Load pigmentation material
	if (auto element = phene_element.find("material"); element != phene_element.end())
		phene.material = resource_manager->load<render::material>(element->get<std::string>());
}

template <>
gene::pigmentation* resource_loader<gene::pigmentation>::load(resource_manager* resource_manager, PHYSFS_File* file, const std::filesystem::path& path)
{
	// Load JSON data
	json* data = resource_loader<json>::load(resource_manager, file, path);
	
	// Validate gene file
	auto pigmentation_element = data->find("pigmentation");
	if (pigmentation_element == data->end())
		throw std::runtime_error("Invalid pigmentation gene.");
	
	// Allocate gene
	gene::pigmentation* pigmentation = new gene::pigmentation();
	
	// Deserialize gene
	gene::deserialize_gene(*pigmentation, &deserialize_pigmentation_phene, *pigmentation_element, resource_manager);
	
	// Free JSON data
	delete data;
	
	return pigmentation;
}

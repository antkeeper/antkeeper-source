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
#include "game/ant/gene/gaster.hpp"
#include "render/model.hpp"
#include <stdexcept>

using namespace game::ant;

static void deserialize_gaster_phene(phene::gaster& phene, const json& phene_element, resource_manager* resource_manager)
{
	phene.model = nullptr;
	phene.phragmosis = 0.0f;
	
	// Load gaster model
	if (auto element = phene_element.find("model"); element != phene_element.end())
		phene.model = resource_manager->load<render::model>(element->get<std::string>());
	
	// Parse phragmosis
	if (auto element = phene_element.find("phragmosis"); element != phene_element.end())
		phene.phragmosis = element->get<float>();
}

template <>
gene::gaster* resource_loader<gene::gaster>::load(resource_manager* resource_manager, PHYSFS_File* file, const std::filesystem::path& path)
{
	// Load JSON data
	json* data = resource_loader<json>::load(resource_manager, file, path);
	
	// Validate gene file
	auto gaster_element = data->find("gaster");
	if (gaster_element == data->end())
		throw std::runtime_error("Invalid gaster gene.");
	
	// Allocate gene
	gene::gaster* gaster = new gene::gaster();
	
	// Deserialize gene
	gene::deserialize_gene(*gaster, &deserialize_gaster_phene, *gaster_element, resource_manager);
	
	// Free JSON data
	delete data;
	
	return gaster;
}

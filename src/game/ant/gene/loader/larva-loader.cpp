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
#include "game/ant/gene/larva.hpp"
#include "render/model.hpp"
#include <stdexcept>

using namespace game::ant;

static void deserialize_larva_phene(phene::larva& phene, const json& phene_element, resource_manager* resource_manager)
{
	phene.model = nullptr;
	phene.instar_count = 0;
	
	// Load larva model
	if (auto element = phene_element.find("model"); element != phene_element.end())
		phene.model = resource_manager->load<render::model>(element->get<std::string>());
	
	// Parse instar count
	if (auto element = phene_element.find("instar_count"); element != phene_element.end())
		phene.instar_count = element->get<int>();
}

template <>
gene::larva* resource_loader<gene::larva>::load(resource_manager* resource_manager, PHYSFS_File* file, const std::filesystem::path& path)
{
	// Load JSON data
	json* data = resource_loader<json>::load(resource_manager, file, path);
	
	// Validate gene file
	auto larva_element = data->find("larva");
	if (larva_element == data->end())
		throw std::runtime_error("Invalid larva gene.");
	
	// Allocate gene
	gene::larva* larva = new gene::larva();
	
	// Deserialize gene
	gene::deserialize_gene(*larva, &deserialize_larva_phene, *larva_element, resource_manager);
	
	// Free JSON data
	delete data;
	
	return larva;
}

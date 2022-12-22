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
#include "game/ant/gene/mandibles.hpp"
#include "render/model.hpp"
#include <stdexcept>

using namespace game::ant;

static void deserialize_mandibles_phene(phene::mandibles& phene, const json& phene_element, resource_manager* resource_manager)
{
	phene.model = nullptr;
	phene.length = 0.0f;
	phene.apical_dental_count = 0;
	phene.basal_dental_count = 0;
	
	// Load mandibles model
	if (auto element = phene_element.find("model"); element != phene_element.end())
		phene.model = resource_manager->load<render::model>(element->get<std::string>());
	
	// Parse length
	if (auto element = phene_element.find("length"); element != phene_element.end())
		phene.length = element->get<float>();
	
	// Parse apical dental count count
	if (auto element = phene_element.find("apical_dental_count"); element != phene_element.end())
		phene.apical_dental_count = element->get<int>();
	
	// Parse basal dental count count
	if (auto element = phene_element.find("basal_dental_count"); element != phene_element.end())
		phene.basal_dental_count = element->get<int>();
}

template <>
gene::mandibles* resource_loader<gene::mandibles>::load(resource_manager* resource_manager, PHYSFS_File* file, const std::filesystem::path& path)
{
	// Load JSON data
	json* data = resource_loader<json>::load(resource_manager, file, path);
	
	// Validate gene file
	auto mandibles_element = data->find("mandibles");
	if (mandibles_element == data->end())
		throw std::runtime_error("Invalid mandibles gene.");
	
	// Allocate gene
	gene::mandibles* mandibles = new gene::mandibles();
	
	// Deserialize gene
	gene::deserialize_gene(*mandibles, &deserialize_mandibles_phene, *mandibles_element, resource_manager);
	
	// Free JSON data
	delete data;
	
	return mandibles;
}

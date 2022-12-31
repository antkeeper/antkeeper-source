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
#include "game/ant/gene/mesosoma.hpp"
#include "render/model.hpp"
#include <stdexcept>

using namespace game::ant;

static void deserialize_mesosoma_phene(phene::mesosoma& phene, const json& phene_element, resource_manager* resource_manager)
{
	phene.model = nullptr;
	phene.pronotum_width = 0.0f;
	phene.pronotum_spinescence = 0.0f;
	phene.mesonotum_spinescence = 0.0f;
	phene.propodeum_spinescence = 0.0f;
	
	// Load mesosoma model
	if (auto element = phene_element.find("model"); element != phene_element.end())
		phene.model = resource_manager->load<render::model>(element->get<std::string>());
	
	// Parse pronotum width
	if (auto element = phene_element.find("pronotum_width"); element != phene_element.end())
		phene.pronotum_width = element->get<float>();
	
	// Parse pronotum spinescence
	if (auto element = phene_element.find("pronotum_spinescence"); element != phene_element.end())
		phene.pronotum_spinescence = element->get<float>();
	
	// Parse mesonotum spinescence
	if (auto element = phene_element.find("mesonotum_spinescence"); element != phene_element.end())
		phene.mesonotum_spinescence = element->get<float>();
	
	// Parse propodeum spinescence
	if (auto element = phene_element.find("propodeum_spinescence"); element != phene_element.end())
		phene.propodeum_spinescence = element->get<float>();
}

template <>
gene::mesosoma* resource_loader<gene::mesosoma>::load(resource_manager* resource_manager, PHYSFS_File* file, const std::filesystem::path& path)
{
	// Load JSON data
	json* data = resource_loader<json>::load(resource_manager, file, path);
	
	// Validate gene file
	auto mesosoma_element = data->find("mesosoma");
	if (mesosoma_element == data->end())
		throw std::runtime_error("Invalid mesosoma gene.");
	
	// Allocate gene
	gene::mesosoma* mesosoma = new gene::mesosoma();
	
	// Deserialize gene
	gene::deserialize_gene(*mesosoma, &deserialize_mesosoma_phene, *mesosoma_element, resource_manager);
	
	// Free JSON data
	delete data;
	
	return mesosoma;
}

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
#include "game/ant/gene/body-size.hpp"
#include <stdexcept>

using namespace ::ant;

static void deserialize_body_size_phene(phene::body_size& phene, const json& phene_element, resource_manager* resource_manager)
{
	phene.min_mesosoma_length = 1.0f;
	phene.max_mesosoma_length = 1.0f;
	phene.mean_mesosoma_length = 1.0f;
	
	// Parse min mesosoma length
	if (auto element = phene_element.find("min_mesosoma_length"); element != phene_element.end())
		phene.min_mesosoma_length = element->get<float>();
	
	// Parse max mesosoma length
	if (auto element = phene_element.find("max_mesosoma_length"); element != phene_element.end())
		phene.max_mesosoma_length = element->get<float>();
	
	// Parse mean mesosoma length
	if (auto element = phene_element.find("mean_mesosoma_length"); element != phene_element.end())
		phene.mean_mesosoma_length = element->get<float>();
}

template <>
gene::body_size* resource_loader<gene::body_size>::load(resource_manager* resource_manager, PHYSFS_File* file, const std::filesystem::path& path)
{
	// Load JSON data
	json* data = resource_loader<json>::load(resource_manager, file, path);
	
	// Validate gene file
	auto body_size_element = data->find("body_size");
	if (body_size_element == data->end())
		throw std::runtime_error("Invalid body size gene.");
	
	// Allocate gene
	gene::body_size* body_size = new gene::body_size();
	
	// Deserialize gene
	gene::deserialize_gene(*body_size, &deserialize_body_size_phene, *body_size_element, resource_manager);
	
	// Free JSON data
	delete data;
	
	return body_size;
}

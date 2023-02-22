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
#include "game/ant/gene/pilosity.hpp"
#include <stdexcept>

using namespace ::ant;

static void deserialize_pilosity_phene(phene::pilosity& phene, const json& phene_element, resource_manager* resource_manager)
{
	phene.density = 0.0f;
	
	// Parse density
	if (auto element = phene_element.find("density"); element != phene_element.end())
		phene.density = element->get<float>();
}

template <>
gene::pilosity* resource_loader<gene::pilosity>::load(resource_manager* resource_manager, PHYSFS_File* file, const std::filesystem::path& path)
{
	// Load JSON data
	json* data = resource_loader<json>::load(resource_manager, file, path);
	
	// Validate gene file
	auto pilosity_element = data->find("pilosity");
	if (pilosity_element == data->end())
		throw std::runtime_error("Invalid pilosity gene.");
	
	// Allocate gene
	gene::pilosity* pilosity = new gene::pilosity();
	
	// Deserialize gene
	gene::deserialize_gene(*pilosity, &deserialize_pilosity_phene, *pilosity_element, resource_manager);
	
	// Free JSON data
	delete data;
	
	return pilosity;
}

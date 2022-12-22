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
#include "game/ant/gene/foraging-time.hpp"
#include "math/angles.hpp"
#include "math/constants.hpp"
#include <stdexcept>

using namespace game::ant;

static void deserialize_foraging_time_phene(phene::foraging_time& phene, const json& phene_element, resource_manager* resource_manager)
{
	phene.min_solar_altitude = -math::half_pi<float>;
	phene.max_solar_altitude = math::half_pi<float>;
	
	// Parse min solar altitude
	if (auto element = phene_element.find("min_solar_altitude"); element != phene_element.end())
		phene.min_solar_altitude = math::radians(element->get<float>());
	
	// Parse max solar altitude
	if (auto element = phene_element.find("max_solar_altitude"); element != phene_element.end())
		phene.max_solar_altitude = math::radians(element->get<float>());
}

template <>
gene::foraging_time* resource_loader<gene::foraging_time>::load(resource_manager* resource_manager, PHYSFS_File* file, const std::filesystem::path& path)
{
	// Load JSON data
	json* data = resource_loader<json>::load(resource_manager, file, path);
	
	// Validate gene file
	auto foraging_time_element = data->find("foraging_time");
	if (foraging_time_element == data->end())
		throw std::runtime_error("Invalid foraging time gene.");
	
	// Allocate gene
	gene::foraging_time* foraging_time = new gene::foraging_time();
	
	// Deserialize gene
	gene::deserialize_gene(*foraging_time, &deserialize_foraging_time_phene, *foraging_time_element, resource_manager);
	
	// Free JSON data
	delete data;
	
	return foraging_time;
}

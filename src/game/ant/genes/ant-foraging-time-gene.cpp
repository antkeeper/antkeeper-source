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

#include "game/ant/genes/ant-gene-loader.hpp"
#include <engine/resources/resource-loader.hpp>
#include <engine/resources/resource-manager.hpp>
#include <engine/utility/json.hpp>
#include "game/ant/genes/ant-foraging-time-gene.hpp"
#include <engine/math/angles.hpp>
#include <engine/math/numbers.hpp>
#include <stdexcept>

static void deserialize_ant_foraging_time_phene(ant_foraging_time_phene& phene, const json& phene_element, resource_manager& resource_manager)
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
std::unique_ptr<ant_foraging_time_gene> resource_loader<ant_foraging_time_gene>::load(::resource_manager& resource_manager, deserialize_context& ctx)
{
	// Load JSON data
	auto json_data = resource_loader<nlohmann::json>::load(resource_manager, ctx);
	
	// Validate gene file
	auto foraging_time_element = json_data->find("foraging_time");
	if (foraging_time_element == json_data->end())
		throw std::runtime_error("Invalid foraging_time gene.");
	
	// Allocate gene
	std::unique_ptr<ant_foraging_time_gene> foraging_time = std::make_unique<ant_foraging_time_gene>();
	
	// Deserialize gene
	deserialize_ant_gene(*foraging_time, &deserialize_ant_foraging_time_phene, *foraging_time_element, resource_manager);
	
	return foraging_time;
}

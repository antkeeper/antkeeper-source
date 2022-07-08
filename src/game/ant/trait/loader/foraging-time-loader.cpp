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

#include "resources/resource-loader.hpp"
#include "resources/resource-manager.hpp"
#include "resources/json.hpp"
#include "game/ant/trait/foraging-time.hpp"
#include "math/angles.hpp"
#include <stdexcept>

using namespace game::ant;

template <>
trait::foraging_time* resource_loader<trait::foraging_time>::load(resource_manager* resource_manager, PHYSFS_File* file, const std::filesystem::path& path)
{
	// Load JSON data
	json* data = resource_loader<json>::load(resource_manager, file, path);
	
	auto foraging_time_element = data->find("foraging_time");
	if (foraging_time_element == data->end())
		throw std::runtime_error("Invalid foraging time trait.");
	
	auto solar_altitude_element = foraging_time_element->find("solar_altitude");
	if (solar_altitude_element == foraging_time_element->end())
		throw std::runtime_error("Foraging time trait doesn't specify solar altitude.");
	
	if (!solar_altitude_element->is_array() || solar_altitude_element->size() != 2)
		throw std::runtime_error("Foraging time trait solar altitude must contain two values.");
	
	trait::foraging_time* foraging_time = new trait::foraging_time();
	foraging_time->solar_altitude_min = math::radians<float>(solar_altitude_element->front().get<float>());
	foraging_time->solar_altitude_max = math::radians<float>(solar_altitude_element->back().get<float>());
	
	// Free JSON data
	delete data;
	
	return foraging_time;
}

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
#include "game/ant/trait/pigmentation.hpp"
#include <stdexcept>

using namespace game::ant;

template <>
trait::pigmentation* resource_loader<trait::pigmentation>::load(resource_manager* resource_manager, PHYSFS_File* file, const std::filesystem::path& path)
{
	// Load JSON data
	json* data = resource_loader<json>::load(resource_manager, file, path);
	
	// Validate trait file
	auto pigmentation_element = data->find("pigmentation");
	if (pigmentation_element == data->end())
		throw std::runtime_error("Invalid pigmentation trait.");
	
	// Allocate pigmentation trait
	trait::pigmentation* pigmentation = new trait::pigmentation();
	
	// Parse pigmentation primary albedo
	pigmentation->primary_albedo = {0.0, 0.0, 0.0};
	if (auto primary_albedo_element = pigmentation_element->find("primary_albedo"); primary_albedo_element != pigmentation_element->end())
	{
		pigmentation->primary_albedo.x = (*primary_albedo_element)[0].get<float>();
		pigmentation->primary_albedo.y = (*primary_albedo_element)[1].get<float>();
		pigmentation->primary_albedo.z = (*primary_albedo_element)[2].get<float>();
	}
	
	// Parse pigmentation secondary albedo
	pigmentation->secondary_albedo = {0.0, 0.0, 0.0};
	if (auto secondary_albedo_element = pigmentation_element->find("secondary_albedo"); secondary_albedo_element != pigmentation_element->end())
	{
		pigmentation->secondary_albedo.x = (*secondary_albedo_element)[0].get<float>();
		pigmentation->secondary_albedo.y = (*secondary_albedo_element)[1].get<float>();
		pigmentation->secondary_albedo.z = (*secondary_albedo_element)[2].get<float>();
	}
	
	// Free JSON data
	delete data;
	
	return pigmentation;
}

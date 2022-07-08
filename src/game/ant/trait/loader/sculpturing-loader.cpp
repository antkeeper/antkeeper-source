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
#include "game/ant/trait/sculpturing.hpp"
#include "gl/texture-2d.hpp"
#include <stdexcept>

using namespace game::ant;

template <>
trait::sculpturing* resource_loader<trait::sculpturing>::load(resource_manager* resource_manager, PHYSFS_File* file, const std::filesystem::path& path)
{
	// Load JSON data
	json* data = resource_loader<json>::load(resource_manager, file, path);
	
	// Validate trait file
	auto sculpturing_element = data->find("sculpturing");
	if (sculpturing_element == data->end())
		throw std::runtime_error("Invalid sculpturing trait.");
	
	// Allocate sculpturing trait
	trait::sculpturing* sculpturing = new trait::sculpturing();
	
	// Load sculpturing normal map
	auto normal_map_element = sculpturing_element->find("normal_map");
	if (normal_map_element == sculpturing_element->end())
		throw std::runtime_error("Sculpturing trait doesn't specify sculpturing normal map.");
	sculpturing->normal_map = resource_manager->load<gl::texture_2d>(normal_map_element->get<std::string>());
	
	// Parse sculpturing roughness
	sculpturing->roughness = 0.0f;
	if (auto roughness_element = sculpturing_element->find("roughness"); roughness_element != sculpturing_element->end())
		sculpturing->roughness = roughness_element->get<float>();
	
	// Free JSON data
	delete data;
	
	return sculpturing;
}

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
#include "game/ant/trait/antennae.hpp"
#include "render/model.hpp"
#include <stdexcept>

using namespace game::ant;

template <>
trait::antennae* resource_loader<trait::antennae>::load(resource_manager* resource_manager, PHYSFS_File* file, const std::filesystem::path& path)
{
	// Load JSON data
	json* data = resource_loader<json>::load(resource_manager, file, path);
	
	// Validate trait file
	auto antennae_element = data->find("antennae");
	if (antennae_element == data->end())
		throw std::runtime_error("Invalid antennae trait.");
	
	// Allocate and init antennae trait
	trait::antennae* antennae = new trait::antennae();
	antennae->model = nullptr;
	antennae->total_antennomere_count = 0;
	antennae->club_antennomere_count = 0;
	
	// Load antennae model
	auto model_element = antennae_element->find("model");
	if (model_element == antennae_element->end())
		throw std::runtime_error("Antennae trait doesn't specify antennae model.");
	antennae->model = resource_manager->load<render::model>(model_element->get<std::string>());
	
	// Parse total antennomere count
	if (auto element = antennae_element->find("total_antennomere_count"); element != antennae_element->end())
		antennae->total_antennomere_count = element->get<int>();
	
	// Parse club antennomere count
	if (auto element = antennae_element->find("club_antennomere_count"); element != antennae_element->end())
		antennae->club_antennomere_count = element->get<int>();
	
	// Free JSON data
	delete data;
	
	return antennae;
}

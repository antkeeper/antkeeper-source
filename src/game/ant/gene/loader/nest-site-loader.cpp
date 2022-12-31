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
#include "game/ant/gene/nest-site.hpp"
#include "math/angles.hpp"
#include "math/constants.hpp"
#include <stdexcept>

using namespace game::ant;

static void deserialize_nest_site_phene(phene::nest_site& phene, const json& phene_element, resource_manager* resource_manager)
{
	
}

template <>
gene::nest_site* resource_loader<gene::nest_site>::load(resource_manager* resource_manager, PHYSFS_File* file, const std::filesystem::path& path)
{
	// Load JSON data
	json* data = resource_loader<json>::load(resource_manager, file, path);
	
	// Validate gene file
	auto nest_site_element = data->find("nest_site");
	if (nest_site_element == data->end())
		throw std::runtime_error("Invalid nest site gene.");
	
	// Allocate gene
	gene::nest_site* nest_site = new gene::nest_site();
	
	// Deserialize gene
	gene::deserialize_gene(*nest_site, &deserialize_nest_site_phene, *nest_site_element, resource_manager);
	
	// Free JSON data
	delete data;
	
	return nest_site;
}

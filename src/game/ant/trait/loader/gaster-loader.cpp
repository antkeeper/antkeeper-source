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
#include "game/ant/trait/gaster.hpp"
#include "render/model.hpp"
#include <stdexcept>

using namespace game::ant;

template <>
trait::gaster* resource_loader<trait::gaster>::load(resource_manager* resource_manager, PHYSFS_File* file, const std::filesystem::path& path)
{
	// Load JSON data
	json* data = resource_loader<json>::load(resource_manager, file, path);
	
	// Validate trait file
	auto gaster_element = data->find("gaster");
	if (gaster_element == data->end())
		throw std::runtime_error("Invalid gaster trait.");
	
	// Allocate and init gaster trait
	trait::gaster* gaster = new trait::gaster();
	gaster->model = nullptr;
	gaster->phragmosis = 0.0;
	
	// Load gaster model
	auto model_element = gaster_element->find("model");
	if (model_element == gaster_element->end())
		throw std::runtime_error("Gaster trait doesn't specify gaster model.");
	gaster->model = resource_manager->load<render::model>(model_element->get<std::string>());
	
	// Parse phragmosis
	if (auto element = gaster_element->find("phragmosis"); element != gaster_element->end())
		gaster->phragmosis = element->get<float>();
	
	// Free JSON data
	delete data;
	
	return gaster;
}

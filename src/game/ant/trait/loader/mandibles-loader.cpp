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
#include "game/ant/trait/mandibles.hpp"
#include "render/model.hpp"
#include <stdexcept>

using namespace game::ant;

template <>
trait::mandibles* resource_loader<trait::mandibles>::load(resource_manager* resource_manager, PHYSFS_File* file, const std::filesystem::path& path)
{
	// Load JSON data
	json* data = resource_loader<json>::load(resource_manager, file, path);
	
	// Validate trait file
	auto mandibles_element = data->find("mandibles");
	if (mandibles_element == data->end())
		throw std::runtime_error("Invalid mandibles trait.");
	
	// Allocate and init mandibles trait
	trait::mandibles* mandibles = new trait::mandibles();
	mandibles->model = nullptr;
	mandibles->length = 0.0f;
	mandibles->apical_dental_count = 0;
	mandibles->basal_dental_count = 0;
	
	// Load model
	auto model_element = mandibles_element->find("model");
	if (model_element == mandibles_element->end())
		throw std::runtime_error("Mandibles trait doesn't specify mandibles model.");
	mandibles->model = resource_manager->load<render::model>(model_element->get<std::string>());
	
	// Parse length
	if (auto element = mandibles_element->find("length"); element != mandibles_element->end())
		mandibles->length = element->get<float>();
	
	// Parse apical dental count
	if (auto element = mandibles_element->find("apical_dental_count"); element != mandibles_element->end())
		mandibles->apical_dental_count = element->get<int>();
	
	// Parse basal dental count
	if (auto element = mandibles_element->find("basal_dental_count"); element != mandibles_element->end())
		mandibles->basal_dental_count = element->get<int>();
	
	// Free JSON data
	delete data;
	
	return mandibles;
}

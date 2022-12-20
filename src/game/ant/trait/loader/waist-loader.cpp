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
#include "game/ant/trait/waist.hpp"
#include "render/model.hpp"
#include <stdexcept>

using namespace game::ant;

template <>
trait::waist* resource_loader<trait::waist>::load(resource_manager* resource_manager, PHYSFS_File* file, const std::filesystem::path& path)
{
	// Load JSON data
	json* data = resource_loader<json>::load(resource_manager, file, path);
	
	// Validate trait file
	auto waist_element = data->find("waist");
	if (waist_element == data->end())
		throw std::runtime_error("Invalid waist trait.");
	
	// Allocate and init waist trait
	trait::waist* waist = new trait::waist();
	waist->model = nullptr;
	waist->petiole_present = false;
	waist->petiole_length = 0.0f;
	waist->petiole_width = 0.0f;
	waist->petiole_height = 0.0f;
	waist->petiole_spinescence = 0.0f;
	waist->postpetiole_present = false;
	waist->postpetiole_length = 0.0f;
	waist->postpetiole_width = 0.0f;
	waist->postpetiole_height = 0.0f;
	waist->postpetiole_spinescence = 0.0f;
	
	// Parse petiole present
	if (auto element = waist_element->find("petiole_present"); element != waist_element->end())
		waist->petiole_present = element->get<bool>();
	
	if (waist->petiole_present)
	{
		// Parse petiole properties
		if (auto element = waist_element->find("petiole_length"); element != waist_element->end())
			waist->petiole_length = element->get<float>();
		if (auto element = waist_element->find("petiole_width"); element != waist_element->end())
			waist->petiole_width = element->get<float>();
		if (auto element = waist_element->find("petiole_height"); element != waist_element->end())
			waist->petiole_height = element->get<float>();
		if (auto element = waist_element->find("petiole_spinescence"); element != waist_element->end())
			waist->petiole_spinescence = element->get<float>();
		
		// Parse postpetiole present
		if (auto element = waist_element->find("postpetiole_present"); element != waist_element->end())
			waist->postpetiole_present = element->get<bool>();
		
		if (waist->postpetiole_present)
		{
			// Parse postpetiole properties
			if (auto element = waist_element->find("postpetiole_length"); element != waist_element->end())
				waist->postpetiole_length = element->get<float>();
			if (auto element = waist_element->find("postpetiole_width"); element != waist_element->end())
				waist->postpetiole_width = element->get<float>();
			if (auto element = waist_element->find("postpetiole_height"); element != waist_element->end())
				waist->postpetiole_height = element->get<float>();
			if (auto element = waist_element->find("postpetiole_spinescence"); element != waist_element->end())
				waist->postpetiole_spinescence = element->get<float>();
		}
		
		// Load waist model
		auto model_element = waist_element->find("model");
		if (model_element == waist_element->end())
			throw std::runtime_error("Waist trait doesn't specify waist model.");
		waist->model = resource_manager->load<render::model>(model_element->get<std::string>());
	}
	
	// Free JSON data
	delete data;
	
	return waist;
}

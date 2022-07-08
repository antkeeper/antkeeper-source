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
#include "game/ant/trait/head.hpp"
#include "render/model.hpp"
#include <stdexcept>

using namespace game::ant;

template <>
trait::head* resource_loader<trait::head>::load(resource_manager* resource_manager, PHYSFS_File* file, const std::filesystem::path& path)
{
	// Load JSON data
	json* data = resource_loader<json>::load(resource_manager, file, path);
	
	// Validate trait file
	auto head_element = data->find("head");
	if (head_element == data->end())
		throw std::runtime_error("Invalid head trait.");
	
	// Allocate head trait
	trait::head* head = new trait::head();
	
	// Load head model
	auto model_element = head_element->find("model");
	if (model_element == head_element->end())
		throw std::runtime_error("Head trait doesn't specify head model.");
	head->model = resource_manager->load<render::model>(model_element->get<std::string>());
	
	// Parse head length
	head->length = 0.0f;
	if (auto length_element = head_element->find("length"); length_element != head_element->end())
		head->length = length_element->get<float>();
	
	// Parse head width
	head->width = 0.0f;
	if (auto width_element = head_element->find("width"); width_element != head_element->end())
		head->width = width_element->get<float>();
	
	// Parse head phragmotic
	head->phragmotic = false;
	if (auto phragmotic_element = head_element->find("phragmotic"); phragmotic_element != head_element->end())
		head->phragmotic = phragmotic_element->get<bool>();
	
	// Free JSON data
	delete data;
	
	return head;
}

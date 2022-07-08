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
	
	// Allocate waist trait
	trait::waist* waist = new trait::waist();
	
	// Load waist model
	auto model_element = waist_element->find("model");
	if (model_element == waist_element->end())
		throw std::runtime_error("Waist trait doesn't specify waist model.");
	waist->model = resource_manager->load<render::model>(model_element->get<std::string>());
	
	// Parse waist spinescence
	waist->spinescence = 0.0f;
	if (auto spinescence_element = waist_element->find("spinescence"); spinescence_element != waist_element->end())
		waist->spinescence = spinescence_element->get<float>();
	
	// Parse waist petiole length
	waist->petiole_length = 0.0f;
	if (auto petiole_length_element = waist_element->find("petiole_length"); petiole_length_element != waist_element->end())
		waist->petiole_length = petiole_length_element->get<float>();
	
	// Parse waist petiole width
	waist->petiole_width = 0.0f;
	if (auto petiole_width_element = waist_element->find("petiole_width"); petiole_width_element != waist_element->end())
		waist->petiole_width = petiole_width_element->get<float>();
		
	// Parse waist petiole height
	waist->petiole_height = 0.0f;
	if (auto petiole_height_element = waist_element->find("petiole_height"); petiole_height_element != waist_element->end())
		waist->petiole_height = petiole_height_element->get<float>();
	
	// Parse waist postpetiole
	waist->postpetiole = false;
	if (auto postpetiole_element = waist_element->find("postpetiole"); postpetiole_element != waist_element->end())
		waist->postpetiole = postpetiole_element->get<bool>();
	
	waist->postpetiole_length = 0.0f;
	waist->postpetiole_width = 0.0f;
	waist->postpetiole_height = 0.0f;

	if (waist->postpetiole)
	{
		// Parse waist postpetiole length
		if (auto postpetiole_length_element = waist_element->find("postpetiole_length"); postpetiole_length_element != waist_element->end())
			waist->postpetiole_length = postpetiole_length_element->get<float>();
		
		// Parse waist postpetiole width
		if (auto postpetiole_width_element = waist_element->find("postpetiole_width"); postpetiole_width_element != waist_element->end())
			waist->postpetiole_width = postpetiole_width_element->get<float>();
			
		// Parse waist postpetiole height
		if (auto postpetiole_height_element = waist_element->find("postpetiole_height"); postpetiole_height_element != waist_element->end())
			waist->postpetiole_height = postpetiole_height_element->get<float>();
	}
	
	// Free JSON data
	delete data;
	
	return waist;
}

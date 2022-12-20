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
#include "game/ant/trait/mesosoma.hpp"
#include "render/model.hpp"
#include <stdexcept>

using namespace game::ant;

template <>
trait::mesosoma* resource_loader<trait::mesosoma>::load(resource_manager* resource_manager, PHYSFS_File* file, const std::filesystem::path& path)
{
	// Load JSON data
	json* data = resource_loader<json>::load(resource_manager, file, path);
	
	// Validate trait file
	auto mesosoma_element = data->find("mesosoma");
	if (mesosoma_element == data->end())
		throw std::runtime_error("Invalid mesosoma trait.");
	
	// Allocate and init mesosoma trait
	trait::mesosoma* mesosoma = new trait::mesosoma();
	mesosoma->model = nullptr;
	mesosoma->pronotum_width = 0.0f;
	mesosoma->pronotum_spinescence = 0.0f;
	mesosoma->mesonotum_spinescence = 0.0f;
	mesosoma->propodeum_spinescence = 0.0f;
	
	// Load mesosoma model
	auto model_element = mesosoma_element->find("model");
	if (model_element == mesosoma_element->end())
		throw std::runtime_error("Mesosoma trait doesn't specify mesosoma model.");
	mesosoma->model = resource_manager->load<render::model>(model_element->get<std::string>());
	
	// Parse pronotum width
	if (auto element = mesosoma_element->find("pronotum_width"); element != mesosoma_element->end())
		mesosoma->pronotum_width = element->get<float>();
	
	// Parse pronotum spinescence
	if (auto element = mesosoma_element->find("pronotum_spinescence"); element != mesosoma_element->end())
		mesosoma->pronotum_spinescence = element->get<float>();
	
	// Parse mesonotum spinescence
	if (auto element = mesosoma_element->find("mesonotum_spinescence"); element != mesosoma_element->end())
		mesosoma->mesonotum_spinescence = element->get<float>();
	
	// Parse propodeum spinescence
	if (auto element = mesosoma_element->find("propodeum_spinescence"); element != mesosoma_element->end())
		mesosoma->propodeum_spinescence = element->get<float>();
	
	// Free JSON data
	delete data;
	
	return mesosoma;
}

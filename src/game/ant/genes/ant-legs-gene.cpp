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

#include "game/ant/genes/ant-gene-loader.hpp"
#include <engine/resources/resource-loader.hpp>
#include <engine/resources/resource-manager.hpp>
#include <engine/utility/json.hpp>
#include "game/ant/genes/ant-legs-gene.hpp"
#include <engine/render/model.hpp>
#include <stdexcept>

static void deserialize_ant_legs_phene(ant_legs_phene& phene, const json& phene_element, resource_manager& resource_manager)
{
	phene.model = nullptr;
	phene.speed = 0.0f;
	phene.grip = 0.0f;
	
	// Load legs model
	if (auto element = phene_element.find("model"); element != phene_element.end())
		phene.model = resource_manager.load<render::model>(element->get<std::string>());
	
	// Parse speed
	if (auto element = phene_element.find("speed"); element != phene_element.end())
		phene.speed = element->get<float>();
	
	// Parse grip
	if (auto element = phene_element.find("grip"); element != phene_element.end())
		phene.grip = element->get<float>();
}

template <>
std::unique_ptr<ant_legs_gene> resource_loader<ant_legs_gene>::load(::resource_manager& resource_manager, deserialize_context& ctx)
{
	// Load JSON data
	auto json_data = resource_loader<nlohmann::json>::load(resource_manager, ctx);
	
	// Validate gene file
	auto legs_element = json_data->find("legs");
	if (legs_element == json_data->end())
		throw std::runtime_error("Invalid legs gene.");
	
	// Allocate gene
	std::unique_ptr<ant_legs_gene> legs = std::make_unique<ant_legs_gene>();
	
	// Deserialize gene
	deserialize_ant_gene(*legs, &deserialize_ant_legs_phene, *legs_element, resource_manager);
	
	return legs;
}

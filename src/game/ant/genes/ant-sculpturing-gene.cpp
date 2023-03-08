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
#include "game/ant/genes/ant-sculpturing-gene.hpp"
#include <engine/gl/texture-2d.hpp>
#include <stdexcept>

static void deserialize_ant_sculpturing_phene(ant_sculpturing_phene& phene, const json& phene_element, resource_manager& resource_manager)
{
	phene.normal_map = nullptr;
	phene.roughness = 0.0f;
	
	// Load normal map
	if (auto element = phene_element.find("normal_map"); element != phene_element.end())
		phene.normal_map = resource_manager.load<gl::texture_2d>(element->get<std::string>());
	
	// Parse roughness
	if (auto element = phene_element.find("roughness"); element != phene_element.end())
		phene.roughness = element->get<float>();
}

template <>
std::unique_ptr<ant_sculpturing_gene> resource_loader<ant_sculpturing_gene>::load(::resource_manager& resource_manager, deserialize_context& ctx)
{
	// Load JSON data
	auto json_data = resource_loader<nlohmann::json>::load(resource_manager, ctx);
	
	// Validate gene file
	auto sculpturing_element = json_data->find("sculpturing");
	if (sculpturing_element == json_data->end())
		throw std::runtime_error("Invalid sculpturing gene.");
	
	// Allocate gene
	std::unique_ptr<ant_sculpturing_gene> sculpturing = std::make_unique<ant_sculpturing_gene>();
	
	// Deserialize gene
	deserialize_ant_gene(*sculpturing, &deserialize_ant_sculpturing_phene, *sculpturing_element, resource_manager);
	
	return sculpturing;
}

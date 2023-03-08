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
#include "game/ant/genes/ant-pigmentation-gene.hpp"
#include <engine/render/material.hpp>
#include <stdexcept>

static void deserialize_ant_pigmentation_phene(ant_pigmentation_phene& phene, const json& phene_element, resource_manager& resource_manager)
{
	phene.material = nullptr;
	
	// Load pigmentation material
	if (auto element = phene_element.find("material"); element != phene_element.end())
		phene.material = resource_manager.load<render::material>(element->get<std::string>());
}

template <>
std::unique_ptr<ant_pigmentation_gene> resource_loader<ant_pigmentation_gene>::load(::resource_manager& resource_manager, deserialize_context& ctx)
{
	// Load JSON data
	auto json_data = resource_loader<nlohmann::json>::load(resource_manager, ctx);
	
	// Validate gene file
	auto pigmentation_element = json_data->find("pigmentation");
	if (pigmentation_element == json_data->end())
		throw std::runtime_error("Invalid pigmentation gene.");
	
	// Allocate gene
	std::unique_ptr<ant_pigmentation_gene> pigmentation = std::make_unique<ant_pigmentation_gene>();
	
	// Deserialize gene
	deserialize_ant_gene(*pigmentation, &deserialize_ant_pigmentation_phene, *pigmentation_element, resource_manager);
	
	return pigmentation;
}

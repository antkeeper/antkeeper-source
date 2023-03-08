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
#include "game/ant/genes/ant-sting-gene.hpp"
#include <engine/render/model.hpp>
#include <stdexcept>

static void deserialize_ant_sting_phene(ant_sting_phene& phene, const json& phene_element, resource_manager& resource_manager)
{
	phene.present = false;
	phene.model = nullptr;
	
	// Parse present
	if (auto element = phene_element.find("present"); element != phene_element.end())
		phene.present = element->get<bool>();
	
	if (phene.present)
	{
		// Load sting model
		if (auto element = phene_element.find("model"); element != phene_element.end())
			phene.model = resource_manager.load<render::model>(element->get<std::string>());
	}
}

template <>
std::unique_ptr<ant_sting_gene> resource_loader<ant_sting_gene>::load(::resource_manager& resource_manager, deserialize_context& ctx)
{
	// Load JSON data
	auto json_data = resource_loader<nlohmann::json>::load(resource_manager, ctx);
	
	// Validate gene file
	auto sting_element = json_data->find("sting");
	if (sting_element == json_data->end())
		throw std::runtime_error("Invalid sting gene.");
	
	// Allocate gene
	std::unique_ptr<ant_sting_gene> sting = std::make_unique<ant_sting_gene>();
	
	// Deserialize gene
	deserialize_ant_gene(*sting, &deserialize_ant_sting_phene, *sting_element, resource_manager);
	
	return sting;
}

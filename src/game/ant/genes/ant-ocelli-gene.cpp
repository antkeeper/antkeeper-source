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
#include "game/ant/genes/ant-ocelli-gene.hpp"
#include <engine/render/model.hpp>
#include <stdexcept>

static void deserialize_ant_ocelli_phene(ant_ocelli_phene& phene, const json& phene_element, resource_manager& resource_manager)
{
	phene.lateral_ocelli_present = false;
	phene.median_ocellus_present = false;
	phene.lateral_ocelli_model = nullptr;
	phene.median_ocellus_model = nullptr;
	phene.width = 0.0f;
	phene.height = 0.0f;
	
	// Parse lateral ocelli present
	if (auto element = phene_element.find("lateral_ocelli_present"); element != phene_element.end())
		phene.lateral_ocelli_present = element->get<bool>();
	
	// Parse median ocelli present
	if (auto element = phene_element.find("median_ocellus_present"); element != phene_element.end())
		phene.median_ocellus_present = element->get<bool>();
	
	// Parse width
	if (auto element = phene_element.find("width"); element != phene_element.end())
		phene.width = element->get<float>();
	
	// Parse height
	if (auto element = phene_element.find("height"); element != phene_element.end())
		phene.height = element->get<float>();
	
	// Load lateral ocelli model, if present
	if (phene.lateral_ocelli_present)
	{
		if (auto element = phene_element.find("lateral_ocelli_model"); element != phene_element.end())
			phene.lateral_ocelli_model = resource_manager.load<render::model>(element->get<std::string>());
	}
	
	// Load median ocellus model, if present
	if (phene.median_ocellus_present)
	{
		if (auto element = phene_element.find("median_ocellus_model"); element != phene_element.end())
			phene.median_ocellus_model = resource_manager.load<render::model>(element->get<std::string>());
	}
}

template <>
std::unique_ptr<ant_ocelli_gene> resource_loader<ant_ocelli_gene>::load(::resource_manager& resource_manager, deserialize_context& ctx)
{
	// Load JSON data
	auto json_data = resource_loader<nlohmann::json>::load(resource_manager, ctx);
	
	// Validate gene file
	auto ocelli_element = json_data->find("ocelli");
	if (ocelli_element == json_data->end())
		throw std::runtime_error("Invalid ocelli gene.");
	
	// Allocate gene
	std::unique_ptr<ant_ocelli_gene> ocelli = std::make_unique<ant_ocelli_gene>();
	
	// Deserialize gene
	deserialize_ant_gene(*ocelli, &deserialize_ant_ocelli_phene, *ocelli_element, resource_manager);
	
	return ocelli;
}

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
#include "game/ant/genes/ant-body-size-gene.hpp"
#include <stdexcept>

static void deserialize_ant_body_size_phene(ant_body_size_phene& phene, const json& phene_element, resource_manager& resource_manager)
{
	phene.min_mesosoma_length = 1.0f;
	phene.max_mesosoma_length = 1.0f;
	phene.mean_mesosoma_length = 1.0f;
	
	// Parse min mesosoma length
	if (auto element = phene_element.find("min_mesosoma_length"); element != phene_element.end())
		phene.min_mesosoma_length = element->get<float>();
	
	// Parse max mesosoma length
	if (auto element = phene_element.find("max_mesosoma_length"); element != phene_element.end())
		phene.max_mesosoma_length = element->get<float>();
	
	// Parse mean mesosoma length
	if (auto element = phene_element.find("mean_mesosoma_length"); element != phene_element.end())
		phene.mean_mesosoma_length = element->get<float>();
}

template <>
std::unique_ptr<ant_body_size_gene> resource_loader<ant_body_size_gene>::load(::resource_manager& resource_manager, deserialize_context& ctx)
{
	// Load JSON data
	auto json_data = resource_loader<nlohmann::json>::load(resource_manager, ctx);
	
	// Validate gene file
	auto body_size_element = json_data->find("body_size");
	if (body_size_element == json_data->end())
		throw std::runtime_error("Invalid body_size gene.");
	
	// Allocate gene
	std::unique_ptr<ant_body_size_gene> body_size = std::make_unique<ant_body_size_gene>();
	
	// Deserialize gene
	deserialize_ant_gene(*body_size, &deserialize_ant_body_size_phene, *body_size_element, resource_manager);
	
	return body_size;
}

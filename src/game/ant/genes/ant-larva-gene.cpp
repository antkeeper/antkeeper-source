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
#include "game/ant/genes/ant-larva-gene.hpp"
#include <engine/render/model.hpp>
#include <stdexcept>

static void deserialize_ant_larva_phene(ant_larva_phene& phene, const json& phene_element, resource_manager& resource_manager)
{
	phene.model = nullptr;
	phene.instar_count = 0;
	
	// Load larva model
	if (auto element = phene_element.find("model"); element != phene_element.end())
		phene.model = resource_manager.load<render::model>(element->get<std::string>());
	
	// Parse instar count
	if (auto element = phene_element.find("instar_count"); element != phene_element.end())
		phene.instar_count = element->get<int>();
}

template <>
std::unique_ptr<ant_larva_gene> resource_loader<ant_larva_gene>::load(::resource_manager& resource_manager, deserialize_context& ctx)
{
	// Load JSON data
	auto json_data = resource_loader<nlohmann::json>::load(resource_manager, ctx);
	
	// Validate gene file
	auto larva_element = json_data->find("larva");
	if (larva_element == json_data->end())
		throw std::runtime_error("Invalid larva gene.");
	
	// Allocate gene
	std::unique_ptr<ant_larva_gene> larva = std::make_unique<ant_larva_gene>();
	
	// Deserialize gene
	deserialize_ant_gene(*larva, &deserialize_ant_larva_phene, *larva_element, resource_manager);
	
	return larva;
}

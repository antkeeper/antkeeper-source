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
#include "game/ant/genes/ant-mesosoma-gene.hpp"
#include <engine/render/model.hpp>
#include <stdexcept>

static void deserialize_ant_mesosoma_phene(ant_mesosoma_phene& phene, const json& phene_element, resource_manager& resource_manager)
{
	phene.model = nullptr;
	phene.pronotum_width = 0.0f;
	phene.pronotum_spinescence = 0.0f;
	phene.mesonotum_spinescence = 0.0f;
	phene.propodeum_spinescence = 0.0f;
	
	// Load mesosoma model
	if (auto element = phene_element.find("model"); element != phene_element.end())
		phene.model = resource_manager.load<render::model>(element->get<std::string>());
	
	// Parse pronotum width
	if (auto element = phene_element.find("pronotum_width"); element != phene_element.end())
		phene.pronotum_width = element->get<float>();
	
	// Parse pronotum spinescence
	if (auto element = phene_element.find("pronotum_spinescence"); element != phene_element.end())
		phene.pronotum_spinescence = element->get<float>();
	
	// Parse mesonotum spinescence
	if (auto element = phene_element.find("mesonotum_spinescence"); element != phene_element.end())
		phene.mesonotum_spinescence = element->get<float>();
	
	// Parse propodeum spinescence
	if (auto element = phene_element.find("propodeum_spinescence"); element != phene_element.end())
		phene.propodeum_spinescence = element->get<float>();
}

template <>
std::unique_ptr<ant_mesosoma_gene> resource_loader<ant_mesosoma_gene>::load(::resource_manager& resource_manager, deserialize_context& ctx)
{
	// Load JSON data
	auto json_data = resource_loader<nlohmann::json>::load(resource_manager, ctx);
	
	// Validate gene file
	auto mesosoma_element = json_data->find("mesosoma");
	if (mesosoma_element == json_data->end())
		throw std::runtime_error("Invalid mesosoma gene.");
	
	// Allocate gene
	std::unique_ptr<ant_mesosoma_gene> mesosoma = std::make_unique<ant_mesosoma_gene>();
	
	// Deserialize gene
	deserialize_ant_gene(*mesosoma, &deserialize_ant_mesosoma_phene, *mesosoma_element, resource_manager);
	
	return mesosoma;
}

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

#ifndef ANTKEEPER_GAME_ANT_GENE_GENE_LOADER_HPP
#define ANTKEEPER_GAME_ANT_GENE_GENE_LOADER_HPP

#include "game/ant/gene/monophenic-gene.hpp"
#include "game/ant/gene/polyphenic-gene.hpp"
#include "resources/json.hpp"
#include "resources/resource-manager.hpp"

namespace game {
namespace ant {
namespace gene {

/**
 * Deserializes a gene.
 *
 * @tparam T Phene type.
 *
 * @param gene Gene to deserialize.
 * @param deserialize_phene Phene deserialization function.
 * @param gene_element JSON element containing a gene definition.
 * @param resource_manager Resource manager pointer.
 */
/// @{
template <class T>
void deserialize_gene(monophenic_gene<T>& gene, void (*deserialize_phene)(T&, const json&, resource_manager*), const json& gene_element, resource_manager* resource_manager)
{
	// Read gene name
	if (auto element = gene_element.find("name"); element != gene_element.end())
		gene.name = element->get<std::string>();
	
	// Deserialize phene
	if (auto element = gene_element.find("phene"); element != gene_element.end())
		deserialize_phene(gene.phene, *element, resource_manager);
}

template <class T>
void deserialize_gene(polyphenic_gene<T>& gene, void (*deserialize_phene)(T&, const json&, resource_manager*), const json& gene_element, resource_manager* resource_manager)
{
	// Read gene name
	if (auto element = gene_element.find("name"); element != gene_element.end())
		gene.name = element->get<std::string>();
	
	// Deserialize phenes
	if (auto phenes_element = gene_element.find("phenes"); phenes_element != gene_element.end())
	{
		if (auto element = phenes_element->find("female"); element != phenes_element->end())
		{
			deserialize_phene(gene.phenes[caste::queen], *element, resource_manager);
			deserialize_phene(gene.phenes[caste::worker], *element, resource_manager);
			deserialize_phene(gene.phenes[caste::soldier], *element, resource_manager);
		}
		if (auto element = phenes_element->find("male"); element != phenes_element->end())
			deserialize_phene(gene.phenes[caste::male], *element, resource_manager);
		if (auto element = phenes_element->find("queen"); element != phenes_element->end())
			deserialize_phene(gene.phenes[caste::queen], *element, resource_manager);
		if (auto element = phenes_element->find("worker"); element != phenes_element->end())
			deserialize_phene(gene.phenes[caste::worker], *element, resource_manager);
		if (auto element = phenes_element->find("soldier"); element != phenes_element->end())
			deserialize_phene(gene.phenes[caste::soldier], *element, resource_manager);
	}
}
/// @}

} // namespace gene
} // namespace ant
} // namespace game

#endif // ANTKEEPER_GAME_ANT_GENE_GENE_LOADER_HPP

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

#include "entity/systems/proteome.hpp"
#include "entity/components/proteome.hpp"
#include "genetics/sequence.hpp"
#include "genetics/standard-code.hpp"

namespace entity {
namespace system {

proteome::proteome(entity::registry& registry):
	updatable(registry)
{
	registry.on_construct<entity::component::genome>().connect<&proteome::on_genome_construct>(this);
	registry.on_replace<entity::component::genome>().connect<&proteome::on_genome_replace>(this);
}

void proteome::update(double t, double dt)
{}

void proteome::on_genome_construct(entity::registry& registry, entity::id entity_id, entity::component::genome& genome)
{
	on_genome_replace(registry, entity_id, genome);
}

void proteome::on_genome_replace(entity::registry& registry, entity::id entity_id, entity::component::genome& genome)
{
	// Allocate a proteome component
	entity::component::proteome proteome_component;
	
	// For each chromosome in the genome
	for (const std::string& chromosome: genome.chromosomes)
	{
		// Find the first ORF in the chromosome
		auto orf = genetics::sequence::find_orf(chromosome.begin(), chromosome.end(), genetics::standard_code);
		
		// While the ORF is valid
		while (orf.start != chromosome.end())
		{
			// Translate the base sequence into an amino acid sequence (protein)
			std::string protein;
			genetics::sequence::translate(orf.start, orf.stop, std::back_inserter(protein), genetics::standard_code);
			
			// Append protein to the proteome
			proteome_component.proteins.push_back(protein);
			
			// Find the next ORF
			orf = genetics::sequence::find_orf(orf.stop, chromosome.end(), genetics::standard_code);
		}
	}
	
	// Assign or replace the entity's proteome component
	registry.assign_or_replace<entity::component::proteome>(entity_id, proteome_component);
}

} // namespace system
} // namespace entity

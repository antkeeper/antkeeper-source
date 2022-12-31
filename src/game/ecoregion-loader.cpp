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

#include "game/ecoregion.hpp"
#include "resources/resource-loader.hpp"
#include "resources/resource-manager.hpp"
#include "resources/json.hpp"
#include <stdexcept>

template <>
game::ecoregion* resource_loader<game::ecoregion>::load(resource_manager* resource_manager, PHYSFS_File* file, const std::filesystem::path& path)
{
	// Load JSON data
	json* data = resource_loader<json>::load(resource_manager, file, path);
	
	// Validate ecoregion file
	auto ecoregion_element = data->find("ecoregion");
	if (ecoregion_element == data->end())
		throw std::runtime_error("Invalid ecoregion file.");
	
	// Allocate and init ecoregion
	game::ecoregion* ecoregion = new game::ecoregion();
	ecoregion->elevation = 0.0f;
	ecoregion->latitude = 0.0f;
	ecoregion->longitude = 0.0f;
	ecoregion->terrain_material = nullptr;
	ecoregion->horizon_material = nullptr;
	
	if (auto element = ecoregion_element->find("name"); element != ecoregion_element->end())
		ecoregion->name = element->get<std::string>();
	
	if (auto location_element = ecoregion_element->find("location"); location_element != ecoregion_element->end())
	{
		if (auto element = location_element->find("elevation"); element != location_element->end())
			ecoregion->elevation = element->get<float>();
		if (auto element = location_element->find("latitude"); element != location_element->end())
			ecoregion->latitude = math::radians(element->get<float>());
		if (auto element = location_element->find("longitude"); element != location_element->end())
			ecoregion->longitude = math::radians(element->get<float>());
	}
	
	if (auto terrain_element = ecoregion_element->find("terrain"); terrain_element != ecoregion_element->end())
	{
		if (auto element = terrain_element->find("material"); element != terrain_element->end())
			ecoregion->terrain_material = resource_manager->load<render::material>(element->get<std::string>());
		if (auto element = terrain_element->find("albedo"); element != terrain_element->end())
			ecoregion->terrain_albedo = {(*element)[0].get<float>(), (*element)[1].get<float>(), (*element)[2].get<float>()};
		if (auto element = terrain_element->find("horizon_material"); element != terrain_element->end())
			ecoregion->horizon_material = resource_manager->load<render::material>(element->get<std::string>());
	}
	
	// Load gene pools
	if (auto gene_pools_element = ecoregion_element->find("gene_pools"); gene_pools_element != ecoregion_element->end())
	{
		// For each gene pool
		for (auto gene_pool_element = gene_pools_element->begin(); gene_pool_element != gene_pools_element->end(); ++gene_pool_element)
		{
			// Allocate gene pool
			ecoregion->gene_pools.resize(ecoregion->gene_pools.size() + 1);
			game::ant::gene_pool& gene_pool = ecoregion->gene_pools.back();
			
			// Read gene pool name
			if (auto name_element = gene_pool_element->find("name"); name_element != gene_pool_element->end())
				gene_pool.name = name_element->get<std::string>();
			
			// Load genes
			if (auto genes_element = gene_pool_element->find("genes"); genes_element != gene_pool_element->end())
			{
				// Load antennae genes
				if (auto antennae_elements = genes_element->find("antennae"); antennae_elements != genes_element->end())
				{
					for (auto antennae_element = antennae_elements->begin(); antennae_element != antennae_elements->end(); ++antennae_element)
					{
						float weight = 0.0f;
						const game::ant::gene::antennae* gene = nullptr;
						
						if (auto weight_element = antennae_element->find("weight"); weight_element != antennae_element->end())
							weight = weight_element->get<float>();
						if (auto gene_element = antennae_element->find("gene"); gene_element != antennae_element->end())
							gene = resource_manager->load<game::ant::gene::antennae>(gene_element->get<std::string>());
						
						if (gene)
						{
							gene_pool.antennae.weights.push_back(weight);
							gene_pool.antennae.genes.push_back(gene);
						}
					}
				}
				
				// Load body size genes
				if (auto body_size_elements = genes_element->find("body_size"); body_size_elements != genes_element->end())
				{
					for (auto body_size_element = body_size_elements->begin(); body_size_element != body_size_elements->end(); ++body_size_element)
					{
						float weight = 0.0f;
						const game::ant::gene::body_size* gene = nullptr;
						
						if (auto weight_element = body_size_element->find("weight"); weight_element != body_size_element->end())
							weight = weight_element->get<float>();
						if (auto gene_element = body_size_element->find("gene"); gene_element != body_size_element->end())
							gene = resource_manager->load<game::ant::gene::body_size>(gene_element->get<std::string>());
						
						if (gene)
						{
							gene_pool.body_size.weights.push_back(weight);
							gene_pool.body_size.genes.push_back(gene);
						}
					}
				}
				
				// Load cocoon genes
				if (auto cocoon_elements = genes_element->find("cocoon"); cocoon_elements != genes_element->end())
				{
					for (auto cocoon_element = cocoon_elements->begin(); cocoon_element != cocoon_elements->end(); ++cocoon_element)
					{
						float weight = 0.0f;
						const game::ant::gene::cocoon* gene = nullptr;
						
						if (auto weight_element = cocoon_element->find("weight"); weight_element != cocoon_element->end())
							weight = weight_element->get<float>();
						if (auto gene_element = cocoon_element->find("gene"); gene_element != cocoon_element->end())
							gene = resource_manager->load<game::ant::gene::cocoon>(gene_element->get<std::string>());
						
						if (gene)
						{
							gene_pool.cocoon.weights.push_back(weight);
							gene_pool.cocoon.genes.push_back(gene);
						}
					}
				}
				
				// Load diet genes
				if (auto diet_elements = genes_element->find("diet"); diet_elements != genes_element->end())
				{
					for (auto diet_element = diet_elements->begin(); diet_element != diet_elements->end(); ++diet_element)
					{
						float weight = 0.0f;
						const game::ant::gene::diet* gene = nullptr;
						
						if (auto weight_element = diet_element->find("weight"); weight_element != diet_element->end())
							weight = weight_element->get<float>();
						if (auto gene_element = diet_element->find("gene"); gene_element != diet_element->end())
							gene = resource_manager->load<game::ant::gene::diet>(gene_element->get<std::string>());
						
						if (gene)
						{
							gene_pool.diet.weights.push_back(weight);
							gene_pool.diet.genes.push_back(gene);
						}
					}
				}
				
				// Load egg genes
				if (auto egg_elements = genes_element->find("egg"); egg_elements != genes_element->end())
				{
					for (auto egg_element = egg_elements->begin(); egg_element != egg_elements->end(); ++egg_element)
					{
						float weight = 0.0f;
						const game::ant::gene::egg* gene = nullptr;
						
						if (auto weight_element = egg_element->find("weight"); weight_element != egg_element->end())
							weight = weight_element->get<float>();
						if (auto gene_element = egg_element->find("gene"); gene_element != egg_element->end())
							gene = resource_manager->load<game::ant::gene::egg>(gene_element->get<std::string>());
						
						if (gene)
						{
							gene_pool.egg.weights.push_back(weight);
							gene_pool.egg.genes.push_back(gene);
						}
					}
				}
				
				// Load eyes genes
				if (auto eyes_elements = genes_element->find("eyes"); eyes_elements != genes_element->end())
				{
					for (auto eyes_element = eyes_elements->begin(); eyes_element != eyes_elements->end(); ++eyes_element)
					{
						float weight = 0.0f;
						const game::ant::gene::eyes* gene = nullptr;
						
						if (auto weight_element = eyes_element->find("weight"); weight_element != eyes_element->end())
							weight = weight_element->get<float>();
						if (auto gene_element = eyes_element->find("gene"); gene_element != eyes_element->end())
							gene = resource_manager->load<game::ant::gene::eyes>(gene_element->get<std::string>());
						
						if (gene)
						{
							gene_pool.eyes.weights.push_back(weight);
							gene_pool.eyes.genes.push_back(gene);
						}
					}
				}
				
				// Load foraging time genes
				if (auto foraging_time_elements = genes_element->find("foraging_time"); foraging_time_elements != genes_element->end())
				{
					for (auto foraging_time_element = foraging_time_elements->begin(); foraging_time_element != foraging_time_elements->end(); ++foraging_time_element)
					{
						float weight = 0.0f;
						const game::ant::gene::foraging_time* gene = nullptr;
						
						if (auto weight_element = foraging_time_element->find("weight"); weight_element != foraging_time_element->end())
							weight = weight_element->get<float>();
						if (auto gene_element = foraging_time_element->find("gene"); gene_element != foraging_time_element->end())
							gene = resource_manager->load<game::ant::gene::foraging_time>(gene_element->get<std::string>());
						
						if (gene)
						{
							gene_pool.foraging_time.weights.push_back(weight);
							gene_pool.foraging_time.genes.push_back(gene);
						}
					}
				}
				
				// Load founding mode genes
				if (auto founding_mode_elements = genes_element->find("founding_mode"); founding_mode_elements != genes_element->end())
				{
					for (auto founding_mode_element = founding_mode_elements->begin(); founding_mode_element != founding_mode_elements->end(); ++founding_mode_element)
					{
						float weight = 0.0f;
						const game::ant::gene::founding_mode* gene = nullptr;
						
						if (auto weight_element = founding_mode_element->find("weight"); weight_element != founding_mode_element->end())
							weight = weight_element->get<float>();
						if (auto gene_element = founding_mode_element->find("gene"); gene_element != founding_mode_element->end())
							gene = resource_manager->load<game::ant::gene::founding_mode>(gene_element->get<std::string>());
						
						if (gene)
						{
							gene_pool.founding_mode.weights.push_back(weight);
							gene_pool.founding_mode.genes.push_back(gene);
						}
					}
				}
				
				// Load gaster genes
				if (auto gaster_elements = genes_element->find("gaster"); gaster_elements != genes_element->end())
				{
					for (auto gaster_element = gaster_elements->begin(); gaster_element != gaster_elements->end(); ++gaster_element)
					{
						float weight = 0.0f;
						const game::ant::gene::gaster* gene = nullptr;
						
						if (auto weight_element = gaster_element->find("weight"); weight_element != gaster_element->end())
							weight = weight_element->get<float>();
						if (auto gene_element = gaster_element->find("gene"); gene_element != gaster_element->end())
							gene = resource_manager->load<game::ant::gene::gaster>(gene_element->get<std::string>());
						
						if (gene)
						{
							gene_pool.gaster.weights.push_back(weight);
							gene_pool.gaster.genes.push_back(gene);
						}
					}
				}
				
				// Load head genes
				if (auto head_elements = genes_element->find("head"); head_elements != genes_element->end())
				{
					for (auto head_element = head_elements->begin(); head_element != head_elements->end(); ++head_element)
					{
						float weight = 0.0f;
						const game::ant::gene::head* gene = nullptr;
						
						if (auto weight_element = head_element->find("weight"); weight_element != head_element->end())
							weight = weight_element->get<float>();
						if (auto gene_element = head_element->find("gene"); gene_element != head_element->end())
							gene = resource_manager->load<game::ant::gene::head>(gene_element->get<std::string>());
						
						if (gene)
						{
							gene_pool.head.weights.push_back(weight);
							gene_pool.head.genes.push_back(gene);
						}
					}
				}
				
				// Load larva genes
				if (auto larva_elements = genes_element->find("larva"); larva_elements != genes_element->end())
				{
					for (auto larva_element = larva_elements->begin(); larva_element != larva_elements->end(); ++larva_element)
					{
						float weight = 0.0f;
						const game::ant::gene::larva* gene = nullptr;
						
						if (auto weight_element = larva_element->find("weight"); weight_element != larva_element->end())
							weight = weight_element->get<float>();
						if (auto gene_element = larva_element->find("gene"); gene_element != larva_element->end())
							gene = resource_manager->load<game::ant::gene::larva>(gene_element->get<std::string>());
						
						if (gene)
						{
							gene_pool.larva.weights.push_back(weight);
							gene_pool.larva.genes.push_back(gene);
						}
					}
				}
				
				// Load legs genes
				if (auto legs_elements = genes_element->find("legs"); legs_elements != genes_element->end())
				{
					for (auto legs_element = legs_elements->begin(); legs_element != legs_elements->end(); ++legs_element)
					{
						float weight = 0.0f;
						const game::ant::gene::legs* gene = nullptr;
						
						if (auto weight_element = legs_element->find("weight"); weight_element != legs_element->end())
							weight = weight_element->get<float>();
						if (auto gene_element = legs_element->find("gene"); gene_element != legs_element->end())
							gene = resource_manager->load<game::ant::gene::legs>(gene_element->get<std::string>());
						
						if (gene)
						{
							gene_pool.legs.weights.push_back(weight);
							gene_pool.legs.genes.push_back(gene);
						}
					}
				}
				
				// Load mandibles genes
				if (auto mandibles_elements = genes_element->find("mandibles"); mandibles_elements != genes_element->end())
				{
					for (auto mandibles_element = mandibles_elements->begin(); mandibles_element != mandibles_elements->end(); ++mandibles_element)
					{
						float weight = 0.0f;
						const game::ant::gene::mandibles* gene = nullptr;
						
						if (auto weight_element = mandibles_element->find("weight"); weight_element != mandibles_element->end())
							weight = weight_element->get<float>();
						if (auto gene_element = mandibles_element->find("gene"); gene_element != mandibles_element->end())
							gene = resource_manager->load<game::ant::gene::mandibles>(gene_element->get<std::string>());
						
						if (gene)
						{
							gene_pool.mandibles.weights.push_back(weight);
							gene_pool.mandibles.genes.push_back(gene);
						}
					}
				}
				
				// Load mesosoma genes
				if (auto mesosoma_elements = genes_element->find("mesosoma"); mesosoma_elements != genes_element->end())
				{
					for (auto mesosoma_element = mesosoma_elements->begin(); mesosoma_element != mesosoma_elements->end(); ++mesosoma_element)
					{
						float weight = 0.0f;
						const game::ant::gene::mesosoma* gene = nullptr;
						
						if (auto weight_element = mesosoma_element->find("weight"); weight_element != mesosoma_element->end())
							weight = weight_element->get<float>();
						if (auto gene_element = mesosoma_element->find("gene"); gene_element != mesosoma_element->end())
							gene = resource_manager->load<game::ant::gene::mesosoma>(gene_element->get<std::string>());
						
						if (gene)
						{
							gene_pool.mesosoma.weights.push_back(weight);
							gene_pool.mesosoma.genes.push_back(gene);
						}
					}
				}
				
				// Load nest site genes
				if (auto nest_site_elements = genes_element->find("nest_site"); nest_site_elements != genes_element->end())
				{
					for (auto nest_site_element = nest_site_elements->begin(); nest_site_element != nest_site_elements->end(); ++nest_site_element)
					{
						float weight = 0.0f;
						const game::ant::gene::nest_site* gene = nullptr;
						
						if (auto weight_element = nest_site_element->find("weight"); weight_element != nest_site_element->end())
							weight = weight_element->get<float>();
						if (auto gene_element = nest_site_element->find("gene"); gene_element != nest_site_element->end())
							gene = resource_manager->load<game::ant::gene::nest_site>(gene_element->get<std::string>());
						
						if (gene)
						{
							gene_pool.nest_site.weights.push_back(weight);
							gene_pool.nest_site.genes.push_back(gene);
						}
					}
				}
				
				// Load ocelli genes
				if (auto ocelli_elements = genes_element->find("ocelli"); ocelli_elements != genes_element->end())
				{
					for (auto ocelli_element = ocelli_elements->begin(); ocelli_element != ocelli_elements->end(); ++ocelli_element)
					{
						float weight = 0.0f;
						const game::ant::gene::ocelli* gene = nullptr;
						
						if (auto weight_element = ocelli_element->find("weight"); weight_element != ocelli_element->end())
							weight = weight_element->get<float>();
						if (auto gene_element = ocelli_element->find("gene"); gene_element != ocelli_element->end())
							gene = resource_manager->load<game::ant::gene::ocelli>(gene_element->get<std::string>());
						
						if (gene)
						{
							gene_pool.ocelli.weights.push_back(weight);
							gene_pool.ocelli.genes.push_back(gene);
						}
					}
				}
				
				// Load pigmentation genes
				if (auto pigmentation_elements = genes_element->find("pigmentation"); pigmentation_elements != genes_element->end())
				{
					for (auto pigmentation_element = pigmentation_elements->begin(); pigmentation_element != pigmentation_elements->end(); ++pigmentation_element)
					{
						float weight = 0.0f;
						const game::ant::gene::pigmentation* gene = nullptr;
						
						if (auto weight_element = pigmentation_element->find("weight"); weight_element != pigmentation_element->end())
							weight = weight_element->get<float>();
						if (auto gene_element = pigmentation_element->find("gene"); gene_element != pigmentation_element->end())
							gene = resource_manager->load<game::ant::gene::pigmentation>(gene_element->get<std::string>());
						
						if (gene)
						{
							gene_pool.pigmentation.weights.push_back(weight);
							gene_pool.pigmentation.genes.push_back(gene);
						}
					}
				}
				
				// Load pilosity genes
				if (auto pilosity_elements = genes_element->find("pilosity"); pilosity_elements != genes_element->end())
				{
					for (auto pilosity_element = pilosity_elements->begin(); pilosity_element != pilosity_elements->end(); ++pilosity_element)
					{
						float weight = 0.0f;
						const game::ant::gene::pilosity* gene = nullptr;
						
						if (auto weight_element = pilosity_element->find("weight"); weight_element != pilosity_element->end())
							weight = weight_element->get<float>();
						if (auto gene_element = pilosity_element->find("gene"); gene_element != pilosity_element->end())
							gene = resource_manager->load<game::ant::gene::pilosity>(gene_element->get<std::string>());
						
						if (gene)
						{
							gene_pool.pilosity.weights.push_back(weight);
							gene_pool.pilosity.genes.push_back(gene);
						}
					}
				}
				
				// Load sculpturing genes
				if (auto sculpturing_elements = genes_element->find("sculpturing"); sculpturing_elements != genes_element->end())
				{
					for (auto sculpturing_element = sculpturing_elements->begin(); sculpturing_element != sculpturing_elements->end(); ++sculpturing_element)
					{
						float weight = 0.0f;
						const game::ant::gene::sculpturing* gene = nullptr;
						
						if (auto weight_element = sculpturing_element->find("weight"); weight_element != sculpturing_element->end())
							weight = weight_element->get<float>();
						if (auto gene_element = sculpturing_element->find("gene"); gene_element != sculpturing_element->end())
							gene = resource_manager->load<game::ant::gene::sculpturing>(gene_element->get<std::string>());
						
						if (gene)
						{
							gene_pool.sculpturing.weights.push_back(weight);
							gene_pool.sculpturing.genes.push_back(gene);
						}
					}
				}
				
				// Load sting genes
				if (auto sting_elements = genes_element->find("sting"); sting_elements != genes_element->end())
				{
					for (auto sting_element = sting_elements->begin(); sting_element != sting_elements->end(); ++sting_element)
					{
						float weight = 0.0f;
						const game::ant::gene::sting* gene = nullptr;
						
						if (auto weight_element = sting_element->find("weight"); weight_element != sting_element->end())
							weight = weight_element->get<float>();
						if (auto gene_element = sting_element->find("gene"); gene_element != sting_element->end())
							gene = resource_manager->load<game::ant::gene::sting>(gene_element->get<std::string>());
						
						if (gene)
						{
							gene_pool.sting.weights.push_back(weight);
							gene_pool.sting.genes.push_back(gene);
						}
					}
				}
				
				// Load waist genes
				if (auto waist_elements = genes_element->find("waist"); waist_elements != genes_element->end())
				{
					for (auto waist_element = waist_elements->begin(); waist_element != waist_elements->end(); ++waist_element)
					{
						float weight = 0.0f;
						const game::ant::gene::waist* gene = nullptr;
						
						if (auto weight_element = waist_element->find("weight"); weight_element != waist_element->end())
							weight = weight_element->get<float>();
						if (auto gene_element = waist_element->find("gene"); gene_element != waist_element->end())
							gene = resource_manager->load<game::ant::gene::waist>(gene_element->get<std::string>());
						
						if (gene)
						{
							gene_pool.waist.weights.push_back(weight);
							gene_pool.waist.genes.push_back(gene);
						}
					}
				}
				
				// Load wings genes
				if (auto wings_elements = genes_element->find("wings"); wings_elements != genes_element->end())
				{
					for (auto wings_element = wings_elements->begin(); wings_element != wings_elements->end(); ++wings_element)
					{
						float weight = 0.0f;
						const game::ant::gene::wings* gene = nullptr;
						
						if (auto weight_element = wings_element->find("weight"); weight_element != wings_element->end())
							weight = weight_element->get<float>();
						if (auto gene_element = wings_element->find("gene"); gene_element != wings_element->end())
							gene = resource_manager->load<game::ant::gene::wings>(gene_element->get<std::string>());
						
						if (gene)
						{
							gene_pool.wings.weights.push_back(weight);
							gene_pool.wings.genes.push_back(gene);
						}
					}
				}
			}
		}
	}
	
	// Free JSON data
	delete data;
	
	return ecoregion;
}

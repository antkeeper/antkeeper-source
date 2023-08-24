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

#include "game/systems/metamorphosis-system.hpp"
#include "game/components/egg-component.hpp"
#include "game/components/larva-component.hpp"
#include "game/components/pupa-component.hpp"
#include "game/components/isometric-growth-component.hpp"
#include "game/components/rigid-body-component.hpp"
#include "game/components/scene-component.hpp"
#include "game/components/ant-genome-component.hpp"
#include <engine/scene/static-mesh.hpp>
#include <engine/scene/skeletal-mesh.hpp>
#include <engine/debug/log.hpp>
#include <execution>

metamorphosis_system::metamorphosis_system(entity::registry& registry):
	updatable_system(registry)
{}

void metamorphosis_system::update(float t, float dt)
{
	// Scale timestep
	const auto scaled_timestep = dt * m_time_scale;
	
	// Incubate eggs
	auto egg_group = registry.group<egg_component>(entt::get<ant_genome_component>);
	std::for_each
	(
		std::execution::seq,
		egg_group.begin(),
		egg_group.end(),
		[&](auto entity_id)
		{
			auto& egg = egg_group.get<egg_component>(entity_id);
			if (egg.incubation_phase >= 1.0f)
			{
				return;
			}
			
			// Advance incubation phase
			egg.incubation_phase += scaled_timestep / egg.incubation_period;
			
			// If incubation complete
			if (egg.incubation_phase >= 1.0f)
			{
				const auto& genome = *egg_group.get<ant_genome_component>(entity_id).genome;
				const auto layer_mask = registry.get<scene_component>(entity_id).layer_mask;
				auto& rigid_body = *registry.get<rigid_body_component>(entity_id).body;
				
				// Calculate scales of first and final instars
				const auto egg_scale = rigid_body.get_scale().x();
				const auto first_instar_scale = egg_scale * genome.larva->phenes.front().first_instar_scale;
				const auto final_instar_scale = egg_scale;
				
				// Calculate larval growth rate
				const auto growth_rate = (final_instar_scale - first_instar_scale) / genome.larva->phenes.front().development_period;
				
				// Erase egg component
				registry.erase<egg_component>(entity_id);
				
				// Replace egg model with larva model
				registry.erase<scene_component>(entity_id);
				registry.emplace<scene_component>(entity_id, std::make_shared<scene::skeletal_mesh>(genome.larva->phenes.front().model), layer_mask);
				
				// Init larva scale
				rigid_body.set_scale(first_instar_scale);
				rigid_body.set_previous_scale(first_instar_scale);
				
				// Define larval development period
				larva_component larva;
				larva.development_period = genome.larva->phenes.front().development_period;
				larva.spinning_period = genome.larva->phenes.front().spinning_period;
				registry.emplace<larva_component>(entity_id, std::move(larva));
				
				// Begin isometric growth
				registry.emplace<isometric_growth_component>(entity_id, growth_rate);
			}
		}
	);
	
	// Develop larvae
	auto larva_group = registry.group<larva_component>(entt::get<ant_genome_component>);
	std::for_each
	(
		std::execution::seq,
		larva_group.begin(),
		larva_group.end(),
		[&](auto entity_id)
		{
			auto& larva = larva_group.get<larva_component>(entity_id);
			
			if (larva.development_phase < 1.0f)
			{
				// Advance larval development phase
				larva.development_phase += scaled_timestep / larva.development_period;
				
				// If larval development complete
				if (larva.development_phase >= 1.0f)
				{
					const auto& rigid_body = *registry.get<rigid_body_component>(entity_id).body;
					const auto& genome = *larva_group.get<ant_genome_component>(entity_id).genome;
					const auto layer_mask = registry.get<scene_component>(entity_id).layer_mask;				
					
					// Halt isometric growth
					registry.remove<isometric_growth_component>(entity_id);
					
					// Construct cocoon mesh
					auto cocoon_mesh = std::make_shared<scene::static_mesh>(genome.pupa->phenes.front().cocoon_model);
					cocoon_mesh->set_transform(rigid_body.get_transform());
					
					// Construct copy of cocoon material
					auto cocoon_material = std::make_shared<render::material>(*cocoon_mesh->get_model()->get_groups().front().material);
					
					// Store cocoon material spinning phase variable
					larva.spinning_phase_matvar = std::static_pointer_cast<render::matvar_float>(cocoon_material->get_variable("spinning_phase"));
					larva.spinning_phase_matvar->set(0.0f);
					
					// Replace cocoon mesh material
					cocoon_mesh->set_material(0, std::move(cocoon_material));
					
					// Construct cocoon entity
					larva.cocoon_eid = registry.create();
					registry.emplace<scene_component>(larva.cocoon_eid, std::move(cocoon_mesh), layer_mask);
				}
			}
			else if (larva.spinning_phase < 1.0f)
			{
				const auto& genome = *larva_group.get<ant_genome_component>(entity_id).genome;
				
				// Advance cocoon-spinning phase
				larva.spinning_phase += scaled_timestep / larva.spinning_period;
				
				// Update spinning phase material variable
				larva.spinning_phase_matvar->set(larva.spinning_phase);
				
				// If cocoon-spinning complete
				if (larva.spinning_phase >= 1.0f)
				{
					// Erase larva component
					registry.erase<larva_component>(entity_id);
					
					// Erase scene component
					registry.erase<scene_component>(entity_id);
					
					// Define pupal development period
					pupa_component pupa;
					pupa.development_period = genome.pupa->phenes.front().development_period;
					registry.emplace<pupa_component>(entity_id, std::move(pupa));
				}
			}
		}
	);
	
	// Develop pupae
	auto pupa_group = registry.group<pupa_component>(entt::get<ant_genome_component>);
	std::for_each
	(
		std::execution::seq,
		pupa_group.begin(),
		pupa_group.end(),
		[&](auto entity_id)
		{
			auto& pupa = pupa_group.get<pupa_component>(entity_id);
			if (pupa.development_phase >= 1.0f)
			{
				return;
			}
			
			// Advance pupal development phase
			pupa.development_phase += scaled_timestep / pupa.development_period;
			
			// If pupal development complete
			if (pupa.development_phase >= 1.0f)
			{
				const auto& genome = *pupa_group.get<ant_genome_component>(entity_id).genome;
				
				// Erase pupa component
				registry.erase<pupa_component>(entity_id);
				
				// Construct adult model
				// registry.emplace<scene_component>(entity_id, std::make_shared<scene::skeletal_mesh>(ant_model), layer_mask);
			}
		}
	);
}

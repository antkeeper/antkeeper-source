// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

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

metamorphosis_system::metamorphosis_system(entity::registry& registry):
	updatable_system(registry)
{}

void metamorphosis_system::update([[maybe_unused]] float t, float dt)
{
	// Scale timestep
	const auto scaled_timestep = dt * m_time_scale;
	
	// Incubate eggs
	auto egg_group = m_registry.group<egg_component>(entt::get<ant_genome_component>);
	for (auto entity_id: egg_group)
	{
		auto& egg = egg_group.get<egg_component>(entity_id);
		if (egg.incubation_phase >= 1.0f)
		{
			continue;
		}
		
		// Advance incubation phase
		egg.incubation_phase += scaled_timestep / egg.incubation_period;
		
		// If incubation complete
		if (egg.incubation_phase >= 1.0f)
		{
			const auto& genome = *egg_group.get<ant_genome_component>(entity_id).genome;
			const auto layer_mask = m_registry.get<scene_component>(entity_id).layer_mask;
			auto& rigid_body = *m_registry.get<rigid_body_component>(entity_id).body;
			
			// Calculate scales of first and final instars
			const auto egg_scale = rigid_body.get_scale().x();
			const auto first_instar_scale = egg_scale * genome.larva->phenes.front().first_instar_scale;
			const auto final_instar_scale = egg_scale;
			
			// Calculate larval growth rate
			const auto growth_rate = (final_instar_scale - first_instar_scale) / genome.larva->phenes.front().development_period;
			
			// Erase egg component
			m_registry.erase<egg_component>(entity_id);
			
			// Replace egg model with larva model
			m_registry.erase<scene_component>(entity_id);
			m_registry.emplace<scene_component>(entity_id, std::make_shared<scene::skeletal_mesh>(genome.larva->phenes.front().model), layer_mask);
			
			// Init larva scale
			rigid_body.set_scale(first_instar_scale);
			rigid_body.set_previous_scale(first_instar_scale);
			
			// Define larval development period
			larva_component larva;
			larva.development_period = genome.larva->phenes.front().development_period;
			larva.spinning_period = genome.larva->phenes.front().spinning_period;
			m_registry.emplace<larva_component>(entity_id, std::move(larva));
			
			// Begin isometric growth
			m_registry.emplace<isometric_growth_component>(entity_id, growth_rate);
		}
	}
	
	// Develop larvae
	auto larva_group = m_registry.group<larva_component>(entt::get<ant_genome_component>);
	for (auto entity_id: larva_group)
	{
		auto& larva = larva_group.get<larva_component>(entity_id);
		
		if (larva.development_phase < 1.0f)
		{
			// Advance larval development phase
			larva.development_phase += scaled_timestep / larva.development_period;
			
			// If larval development complete
			if (larva.development_phase >= 1.0f)
			{
				const auto& rigid_body = *m_registry.get<rigid_body_component>(entity_id).body;
				const auto& genome = *larva_group.get<ant_genome_component>(entity_id).genome;
				const auto layer_mask = m_registry.get<scene_component>(entity_id).layer_mask;				
				
				// Halt isometric growth
				m_registry.remove<isometric_growth_component>(entity_id);
				
				// Construct cocoon mesh
				auto cocoon_mesh = std::make_shared<scene::static_mesh>(genome.pupa->phenes.front().cocoon_model);
				cocoon_mesh->set_transform(rigid_body.get_transform());
				
				// Construct copy of cocoon material
				auto cocoon_material = std::make_shared<render::material>(*cocoon_mesh->get_model()->materials().front());
				
				// Store cocoon material spinning phase variable
				larva.spinning_phase_matvar = std::static_pointer_cast<render::matvar_float>(cocoon_material->get_variable("spinning_phase"));
				larva.spinning_phase_matvar->set(0.0f);
				
				// Replace cocoon mesh material
				cocoon_mesh->set_material(0, std::move(cocoon_material));
				
				// Construct cocoon entity
				larva.cocoon_eid = m_registry.create();
				m_registry.emplace<scene_component>(larva.cocoon_eid, std::move(cocoon_mesh), layer_mask);
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
				m_registry.erase<larva_component>(entity_id);
				
				// Erase scene component
				m_registry.erase<scene_component>(entity_id);
				
				// Define pupal development period
				pupa_component pupa;
				pupa.development_period = genome.pupa->phenes.front().development_period;
				m_registry.emplace<pupa_component>(entity_id, std::move(pupa));
			}
		}
	}
	
	// Develop pupae
	auto pupa_group = m_registry.group<pupa_component>(entt::get<ant_genome_component>);
	for (auto entity_id: pupa_group)
	{
		auto& pupa = pupa_group.get<pupa_component>(entity_id);
		if (pupa.development_phase >= 1.0f)
		{
			continue;
		}
		
		// Advance pupal development phase
		pupa.development_phase += scaled_timestep / pupa.development_period;
		
		// If pupal development complete
		if (pupa.development_phase >= 1.0f)
		{
			// const auto& genome = *pupa_group.get<ant_genome_component>(entity_id).genome;
			
			// Erase pupa component
			m_registry.erase<pupa_component>(entity_id);
			
			// Construct adult model
			// m_registry.emplace<scene_component>(entity_id, std::make_shared<scene::skeletal_mesh>(ant_model), layer_mask);
		}
	}
}

// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include "game/systems/metabolic-system.hpp"
#include "game/components/isometric-growth-component.hpp"
#include "game/components/rigid-body-component.hpp"

metabolic_system::metabolic_system(entity::registry& registry):
	updatable_system(registry)
{}

void metabolic_system::update([[maybe_unused]] float t, float dt)
{
	// Scale timestep
	const auto scaled_timestep = dt * m_time_scale;
	
	// Handle isometric growth
	auto isometric_growth_group = m_registry.group<isometric_growth_component>(entt::get<rigid_body_component>);
	for (auto entity_id: isometric_growth_group)
	{
		auto& growth = isometric_growth_group.get<isometric_growth_component>(entity_id);
		auto& rigid_body = *isometric_growth_group.get<rigid_body_component>(entity_id).body;
		
		rigid_body.set_scale(rigid_body.get_scale() + growth.rate * scaled_timestep);
	}
}

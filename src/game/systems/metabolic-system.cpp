// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include "game/systems/metabolic-system.hpp"
#include "game/components/isometric-growth-component.hpp"
#include "game/components/rigid-body-component.hpp"
#include "game/utility/time.hpp"

void metabolic_system::fixed_update(entity::registry& registry, float, float dt)
{
	// Scale timestep
	const auto time_scale = get_time_scale(registry);
	const auto scaled_timestep = dt * time_scale;
	
	// Handle isometric growth
	auto isometric_growth_group = registry.group<isometric_growth_component>(entt::get<rigid_body_component>);
	for (auto entity_id: isometric_growth_group)
	{
		auto& growth = isometric_growth_group.get<isometric_growth_component>(entity_id);
		auto& rigid_body = *isometric_growth_group.get<rigid_body_component>(entity_id).body;
		
		rigid_body.set_scale(rigid_body.get_scale() + growth.rate * scaled_timestep);
	}
}

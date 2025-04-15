// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <execution>
#include <entt/entt.hpp>
#include "game/systems/ik-system.hpp"
#include "game/components/ik-component.hpp"
import engine.entity.id;
import <algorithm>;

void ik_system::fixed_update(entity::registry& registry, float, float)
{
	auto view = registry.view<ik_component>();
	std::for_each
	(
		std::execution::par_unseq,
		view.begin(),
		view.end(),
		[&](auto entity_id)
		{
			const auto& component = view.get<ik_component>(entity_id);
			
			component.rig->solve();
		}
	);
}

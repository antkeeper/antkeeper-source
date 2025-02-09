// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include "game/systems/spatial-system.hpp"
#include "game/components/transform-component.hpp"
#include "game/components/constraint-stack-component.hpp"

void spatial_system::fixed_update(entity::registry& registry, float, float)
{
	// TODO: Only update world transforms if the local transform has changed
	const auto view = registry.view<transform_component>(entt::exclude<constraint_stack_component>);
	for (auto entity_id: view)
	{
		auto& transform = view.get<transform_component>(entity_id);
		transform.world = transform.local;
	}
}

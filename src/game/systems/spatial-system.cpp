// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include "game/systems/spatial-system.hpp"
#include "game/components/transform-component.hpp"
#include "game/components/constraint-stack-component.hpp"
#include <algorithm>
#include <execution>

spatial_system::spatial_system(entity::registry& registry):
	updatable_system(registry),
	updated_unconstrained_transforms(registry, entt::collector.update<transform_component>().where(entt::exclude<constraint_stack_component>))
{}

void spatial_system::update(float t, float dt)
{
	// Update world-space transforms of all updated, unconstrained transform components
	std::for_each
	(
		std::execution::par_unseq,
		updated_unconstrained_transforms.begin(),
		updated_unconstrained_transforms.end(),
		[&](auto entity_id)
		{
			auto& transform = registry.get<transform_component>(entity_id);
			transform.world = transform.local;
		}
	);
	updated_unconstrained_transforms.clear();
}

/*
 * Copyright (C) 2020  Christopher J. Howard
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

#include "constraint-system.hpp"
#include "game/components/copy-translation-component.hpp"
#include "game/components/copy-rotation-component.hpp"
#include "game/components/copy-scale-component.hpp"
#include "game/components/copy-transform-component.hpp"
#include "game/components/transform-component.hpp"
#include "utility/fundamental-types.hpp"

using namespace ecs;

constraint_system::constraint_system(entt::registry& registry):
	entity_system(registry)
{}

void constraint_system::update(double t, double dt)
{
	auto transforms_view = registry.view<transform_component>();
	
	// Handle copy translation constraints
	registry.view<copy_translation_component, transform_component>().each
	(
		[&](auto entity, auto& constraint, auto& transform)
		{
			if (registry.has<transform_component>(constraint.target))
			{
				const float3& target_translation = transforms_view.get(constraint.target).world.translation;
				if (constraint.use_x)
					transform.world.translation.x = (constraint.invert_x) ? -target_translation.x : target_translation.x;
				if (constraint.use_y)
					transform.world.translation.y = (constraint.invert_y) ? -target_translation.y : target_translation.y;
				if (constraint.use_z)
					transform.world.translation.z = (constraint.invert_z) ? -target_translation.z : target_translation.z;
			}
		}
	);
	
	// Handle copy rotation constraints
	registry.view<copy_rotation_component, transform_component>().each
	(
		[&](auto entity, auto& constraint, auto& transform)
		{
			if (registry.has<transform_component>(constraint.target))
			{
				transform.world.rotation = transforms_view.get(constraint.target).world.rotation;
			}
		}
	);
	
	// Handle copy scale constraints
	registry.view<copy_scale_component, transform_component>().each
	(
		[&](auto entity, auto& constraint, auto& transform)
		{
			if (registry.has<transform_component>(constraint.target))
			{
				const float3& target_scale = transforms_view.get(constraint.target).world.scale;
				if (constraint.use_x)
					transform.world.scale.x = target_scale.x;
				if (constraint.use_y)
					transform.world.scale.y = target_scale.y;
				if (constraint.use_z)
					transform.world.scale.z = target_scale.z;
			}
		}
	);
	
	// Handle copy transform constraints
	registry.view<copy_transform_component, transform_component>().each
	(
		[&](auto entity, auto& constraint, auto& transform)
		{
			if (registry.has<transform_component>(constraint.target))
			{
				transform.world = transforms_view.get(constraint.target).world;
			}
		}
	);
}

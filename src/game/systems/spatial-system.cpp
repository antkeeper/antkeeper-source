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

#include "game/systems/spatial-system.hpp"
#include "game/components/transform-component.hpp"
#include "game/components/constraint-stack-component.hpp"


spatial_system::spatial_system(entity::registry& registry):
	updatable_system(registry),
	updated_unconstrained_transforms(registry, entt::collector.update<transform_component>().where(entt::exclude<constraint_stack_component>))
{}

void spatial_system::update(double t, double dt)
{
	// Update world-space transforms of all updated, unconstrained transforms
	for (const auto transform_eid: updated_unconstrained_transforms)
	{
		auto& transform = registry.get<transform_component>(transform_eid);
		transform.world = transform.local;
	}
	updated_unconstrained_transforms.clear();
}


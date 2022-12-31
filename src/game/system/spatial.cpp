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

#include "spatial.hpp"
#include "game/component/transform.hpp"
#include "game/component/constraint-stack.hpp"
#include <iostream>

namespace game {
namespace system {

spatial::spatial(entity::registry& registry):
	updatable(registry),
	updated_unconstrained_transforms(registry, entt::collector.update<component::transform>().where(entt::exclude<component::constraint_stack>))
{}

void spatial::update(double t, double dt)
{
	// Update world-space transforms of all updated, unconstrained transforms
	for (const auto transform_eid: updated_unconstrained_transforms)
	{
		auto& transform = registry.get<component::transform>(transform_eid);
		transform.world = transform.local;
	}
	updated_unconstrained_transforms.clear();
}

} // namespace system
} // namespace game

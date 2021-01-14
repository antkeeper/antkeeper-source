/*
 * Copyright (C) 2021  Christopher J. Howard
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

#include "locomotion-system.hpp"
#include "ecs/components/collision-component.hpp"
#include "ecs/components/locomotion-component.hpp"
#include "ecs/components/transform-component.hpp"
#include "ecs/entity.hpp"

namespace ecs {

locomotion_system::locomotion_system(ecs::registry& registry):
	entity_system(registry)
{}

void locomotion_system::update(double t, double dt)
{
	registry.view<transform_component, locomotion_component>().each(
		[&](ecs::entity entity, auto& transform, auto& locomotion)
		{
		});
}

} // namespace ecs

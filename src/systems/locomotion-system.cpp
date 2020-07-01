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

#include "locomotion-system.hpp"
#include "entity/components/collision-component.hpp"
#include "entity/components/locomotion-component.hpp"
#include "entity/components/transform-component.hpp"

using namespace ecs;

locomotion_system::locomotion_system(entt::registry& registry):
	entity_system(registry)
{}

void locomotion_system::update(double t, double dt)
{
	registry.view<transform_component, locomotion_component>().each(
		[&](auto entity, auto& transform, auto& locomotion)
		{
		});
}


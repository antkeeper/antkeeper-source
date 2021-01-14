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

#include "collision-system.hpp"
#include "ecs/components/transform-component.hpp"

namespace ecs {

collision_system::collision_system(ecs::registry& registry):
	entity_system(registry)
{
	registry.on_construct<collision_component>().connect<&collision_system::on_collision_construct>(this);
	registry.on_replace<collision_component>().connect<&collision_system::on_collision_replace>(this);
	registry.on_destroy<collision_component>().connect<&collision_system::on_collision_destroy>(this);
}

void collision_system::update(double t, double dt)
{}

void collision_system::on_collision_construct(ecs::registry& registry, ecs::entity entity, collision_component& collision)
{}

void collision_system::on_collision_replace(ecs::registry& registry, ecs::entity entity, collision_component& collision)
{}

void collision_system::on_collision_destroy(ecs::registry& registry, ecs::entity entity)
{}

} // namespace ecs

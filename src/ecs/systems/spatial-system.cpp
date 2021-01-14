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

#include "spatial-system.hpp"
#include "ecs/components/parent-component.hpp"
#include "ecs/components/transform-component.hpp"

namespace ecs {

spatial_system::spatial_system(ecs::registry& registry):
	entity_system(registry)
{}

void spatial_system::update(double t, double dt)
{
	/// @TODO: sort transforms by parent, for more multi-level hierarchies
	
	// Process parent transforms first
	registry.view<transform_component>().each(
		[&](ecs::entity entity, auto& transform)
		{
			if (!this->registry.has<parent_component>(entity))
			{
				transform.world = transform.local;
			}
		});
	
	// Process child transforms second
	registry.view<transform_component>().each(
		[&](ecs::entity entity, auto& transform)
		{
			if (this->registry.has<parent_component>(entity))
			{
				ecs::entity parent = this->registry.get<parent_component>(entity).parent;
				const transform_component& parent_transform = this->registry.get<transform_component>(parent);
				transform.world = parent_transform.world * transform.local;
				transform.warp = parent_transform.warp;
			}
		});
}

} // namespace ecs

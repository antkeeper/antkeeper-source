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

#include "spatial.hpp"
#include "entity/components/parent.hpp"
#include "entity/components/transform.hpp"

namespace entity {
namespace system {

spatial::spatial(entity::registry& registry):
	updatable(registry)
{}

void spatial::update(double t, double dt)
{
	/// @TODO: sort transforms by parent, for more multi-level hierarchies
	
	// Process parent transforms first
	registry.view<component::transform>().each(
		[&](entity::id entity_id, auto& transform)
		{
			if (!this->registry.has<component::parent>(entity_id))
			{
				transform.world = transform.local;
			}
		});
	
	// Process child transforms second
	registry.view<component::transform>().each(
		[&](entity::id entity_id, auto& transform)
		{
			if (this->registry.has<component::parent>(entity_id))
			{
				entity::id parent = this->registry.get<component::parent>(entity_id).parent;
				const component::transform& parent_transform = this->registry.get<component::transform>(parent);
				transform.world = parent_transform.world * transform.local;
				transform.warp = parent_transform.warp;
			}
		});
}

} // namespace system
} // namespace entity

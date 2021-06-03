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

#include "constraint.hpp"
#include "entity/components/copy-translation.hpp"
#include "entity/components/copy-rotation.hpp"
#include "entity/components/copy-scale.hpp"
#include "entity/components/copy-transform.hpp"
#include "entity/components/transform.hpp"
#include "utility/fundamental-types.hpp"

namespace entity {
namespace system {

constraint::constraint(entity::registry& registry):
	updatable(registry)
{}

void constraint::update(double t, double dt)
{
	auto transforms_view = registry.view<component::transform>();
	
	// Handle copy translation constraints
	registry.view<component::copy_translation, component::transform>().each
	(
		[&](entity::id entity_id, auto& constraint, auto& transform)
		{
			if (this->registry.has<component::transform>(constraint.target))
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
	registry.view<component::copy_rotation, component::transform>().each
	(
		[&](entity::id entity_id, auto& constraint, auto& transform)
		{
			if (this->registry.has<component::transform>(constraint.target))
			{
				transform.world.rotation = transforms_view.get(constraint.target).world.rotation;
			}
		}
	);
	
	// Handle copy scale constraints
	registry.view<component::copy_scale, component::transform>().each
	(
		[&](entity::id entity_id, auto& constraint, auto& transform)
		{
			if (this->registry.has<component::transform>(constraint.target))
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
	registry.view<component::copy_transform, component::transform>().each
	(
		[&](entity::id entity_id, auto& constraint, auto& transform)
		{
			if (this->registry.has<component::transform>(constraint.target))
			{
				transform.world = transforms_view.get(constraint.target).world;
			}
		}
	);
}

} // namespace system
} // namespace entity

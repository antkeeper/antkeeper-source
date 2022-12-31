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

#include "entity/commands.hpp"
#include "game/component/model.hpp"
#include "game/component/transform.hpp"
#include "game/component/parent.hpp"
#include "game/component/celestial-body.hpp"
#include "game/component/terrain.hpp"
#include "math/quaternion.hpp"
#include <limits>

namespace entity {
namespace command {

void translate(entity::registry& registry, entity::id eid, const float3& translation)
{
	const game::component::transform* transform = registry.try_get<game::component::transform>(eid);
	if (transform)
	{
		registry.patch<game::component::transform>
		(
			eid,
			[&translation](auto& transform)
			{
				transform.local.translation += translation;
			}
		);
	}
}

void rotate(entity::registry& registry, entity::id eid, float angle, const float3& axis)
{
	const game::component::transform* transform = registry.try_get<game::component::transform>(eid);
	if (transform)
	{
		registry.patch<game::component::transform>
		(
			eid,
			[angle, &axis](auto& transform)
			{
				transform.local.rotation = math::normalize(math::angle_axis(angle, axis) * transform.local.rotation);
			}
		);
	}
}

void move_to(entity::registry& registry, entity::id eid, const float3& position)
{
	const game::component::transform* transform = registry.try_get<game::component::transform>(eid);
	if (transform)
	{
		registry.patch<game::component::transform>
		(
			eid,
			[&position](auto& transform)
			{
				transform.local.translation = position;
			}
		);
	}
}

void warp_to(entity::registry& registry, entity::id eid, const float3& position)
{
	const game::component::transform* transform = registry.try_get<game::component::transform>(eid);
	if (transform)
	{
		registry.patch<game::component::transform>
		(
			eid,
			[&position](auto& transform)
			{
				transform.local.translation = position;
				transform.warp = true;
			}
		);
	}
}

void set_scale(entity::registry& registry, entity::id eid, const float3& scale)
{
	const game::component::transform* transform = registry.try_get<game::component::transform>(eid);
	if (transform)
	{
		registry.patch<game::component::transform>
		(
			eid,
			[&scale](auto& transform)
			{
				transform.local.scale = scale;
			}
		);
	}
}

void set_transform(entity::registry& registry, entity::id eid, const math::transform<float>& transform, bool warp)
{
	const game::component::transform* transform_component = registry.try_get<game::component::transform>(eid);
	if (transform_component)
	{
		registry.patch<game::component::transform>
		(
			eid,
			[&other_transform = transform, warp](auto& transform)
			{
				transform.local = other_transform;
				transform.warp = warp;
			}
		);
	}
}

void place(entity::registry& registry, entity::id eid, entity::id celestial_body_id, double altitude, double latitude, double longitude)
{
	/*
	if (registry.has<game::component::transform>(eid))
	{
		double x = 0.0;
		double y = altitude;
		double z = 0.0;
		
		if (registry.has<game::component::celestial_body>(celestial_body_id))
		{
			const game::component::celestial_body& celestial_body = registry.get<game::component::celestial_body>(celestial_body_id);
			
			x = longitude * math::two_pi<double> * celestial_body.radius;
			z = -latitude * math::two_pi<double> * celestial_body.radius;
			
			if (registry.has<game::component::terrain>(celestial_body_id))
			{
				const game::component::terrain& terrain = registry.get<game::component::terrain>(celestial_body_id);
				
				if (terrain.elevation != nullptr)
				{
					y += terrain.elevation(latitude, longitude);
				}
			}
		}
		
		game::component::transform& transform = registry.get<game::component::transform>(eid);
		transform.local.translation = math::vector<float, 3>(double3{x, y, z});
		transform.warp = true;
	}
	*/
}

void assign_render_layers(entity::registry& registry, entity::id eid, unsigned int layers)
{
	const game::component::model* model = registry.try_get<game::component::model>(eid);
	if (model)
	{
		registry.patch<game::component::model>
		(
			eid,
			[layers](auto& model)
			{
				model.layers = layers;
			}
		);
	}
}

math::transform<float> get_local_transform(entity::registry& registry, entity::id eid)
{
	const game::component::transform* transform = registry.try_get<game::component::transform>(eid);
	if (transform)
	{
		return transform->local;
	}
	
	return math::transform<float>::identity;
}

math::transform<float> get_world_transform(entity::registry& registry, entity::id eid)
{
	const game::component::transform* transform = registry.try_get<game::component::transform>(eid);
	if (transform)
	{
		return transform->world;
	}
	
	return math::transform<float>::identity;
}

} // namespace command
} // namespace entity

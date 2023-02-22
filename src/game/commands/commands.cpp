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

#include "game/commands/commands.hpp"
#include "game/components/model-component.hpp"
#include "game/components/transform-component.hpp"
#include "game/components/celestial-body-component.hpp"
#include "game/components/terrain-component.hpp"
#include <engine/math/quaternion.hpp>
#include <limits>

namespace command {

void translate(entity::registry& registry, entity::id eid, const float3& translation)
{
	const ::transform_component* transform = registry.try_get<::transform_component>(eid);
	if (transform)
	{
		registry.patch<::transform_component>
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
	const ::transform_component* transform = registry.try_get<::transform_component>(eid);
	if (transform)
	{
		registry.patch<::transform_component>
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
	const ::transform_component* transform = registry.try_get<::transform_component>(eid);
	if (transform)
	{
		registry.patch<::transform_component>
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
	const ::transform_component* transform = registry.try_get<::transform_component>(eid);
	if (transform)
	{
		registry.patch<::transform_component>
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
	const ::transform_component* transform = registry.try_get<::transform_component>(eid);
	if (transform)
	{
		registry.patch<::transform_component>
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
	const ::transform_component* transform_component = registry.try_get<::transform_component>(eid);
	if (transform_component)
	{
		registry.patch<::transform_component>
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

}

void assign_render_layers(entity::registry& registry, entity::id eid, unsigned int layers)
{
	const ::model_component* model = registry.try_get<::model_component>(eid);
	if (model)
	{
		registry.patch<::model_component>
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
	const ::transform_component* transform = registry.try_get<::transform_component>(eid);
	if (transform)
	{
		return transform->local;
	}
	
	return math::transform<float>::identity;
}

math::transform<float> get_world_transform(entity::registry& registry, entity::id eid)
{
	const ::transform_component* transform = registry.try_get<::transform_component>(eid);
	if (transform)
	{
		return transform->world;
	}
	
	return math::transform<float>::identity;
}

} // namespace command

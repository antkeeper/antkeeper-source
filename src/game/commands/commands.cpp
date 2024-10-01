// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include "game/commands/commands.hpp"
#include "game/components/scene-component.hpp"
#include "game/components/transform-component.hpp"
#include "game/components/celestial-body-component.hpp"
#include "game/components/terrain-component.hpp"
#include <engine/math/quaternion.hpp>
#include <limits>

namespace command {

void translate(entity::registry& registry, entity::id eid, const math::fvec3& translation)
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

void rotate(entity::registry& registry, entity::id eid, float angle, const math::fvec3& axis)
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

void move_to(entity::registry& registry, entity::id eid, const math::fvec3& position)
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

void warp_to(entity::registry& registry, entity::id eid, const math::fvec3& position)
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

void set_scale(entity::registry& registry, entity::id eid, const math::fvec3& scale)
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

void set_transform(entity::registry& registry, entity::id eid, const math::transform<float>& transform)
{
	const ::transform_component* transform_component = registry.try_get<::transform_component>(eid);
	if (transform_component)
	{
		registry.patch<::transform_component>
		(
			eid,
			[&other_transform = transform](auto& transform)
			{
				transform.local = other_transform;
			}
		);
	}
}

void assign_render_layers(entity::registry& registry, entity::id eid, std::uint8_t layer_mask)
{
	const ::scene_component* component = registry.try_get<::scene_component>(eid);
	if (component)
	{
		registry.patch<::scene_component>
		(
			eid,
			[layer_mask](auto& component)
			{
				component.layer_mask = layer_mask;
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
	
	return math::identity<math::transform<float>>;
}

math::transform<float> get_world_transform(entity::registry& registry, entity::id eid)
{
	const ::transform_component* transform = registry.try_get<::transform_component>(eid);
	if (transform)
	{
		return transform->world;
	}
	
	return math::identity<math::transform<float>>;
}

} // namespace command

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

#include "game/entity-commands.hpp"
#include "game/components/model-component.hpp"
#include "game/components/transform-component.hpp"
#include "game/components/copy-transform-component.hpp"
#include "game/components/snap-component.hpp"
#include "game/components/parent-component.hpp"
#include <limits>

namespace ec {
	
using namespace ecs;

void translate(entt::registry& registry, entt::entity eid, const float3& translation)
{
	if (registry.has<transform_component>(eid))
	{
		transform_component& transform = registry.get<transform_component>(eid);
		transform.local.translation += translation;
	}
}

void move_to(entt::registry& registry, entt::entity eid, const float3& position)
{
	if (registry.has<transform_component>(eid))
	{
		transform_component& transform = registry.get<transform_component>(eid);
		transform.local.translation = position;
	}
}

void warp_to(entt::registry& registry, entt::entity eid, const float3& position)
{
	if (registry.has<transform_component>(eid))
	{
		transform_component& transform = registry.get<transform_component>(eid);
		transform.local.translation = position;
		transform.warp = true;
	}
}

void set_scale(entt::registry& registry, entt::entity eid, const float3& scale)
{
	if (registry.has<transform_component>(eid))
	{
		transform_component& transform = registry.get<transform_component>(eid);
		transform.local.scale = scale;
	}
}

void set_transform(entt::registry& registry, entt::entity eid, const math::transform<float>& transform, bool warp)
{
	if (registry.has<transform_component>(eid))
	{
		transform_component& component = registry.get<transform_component>(eid);
		component.local = transform;
		component.warp = warp;
	}
}

void place(entt::registry& registry, entt::entity eid, const float2& translation)
{
	snap_component component;
	component.warp = true;
	component.relative = false;
	component.autoremove = true;
	component.ray.origin = {translation[0], 10000.0f, translation[1]};
	component.ray.direction = {0.0f, -1.0f, 0.0f};
	registry.assign_or_replace<snap_component>(eid, component);
}

void assign_render_layers(entt::registry& registry, entt::entity eid, unsigned int layers)
{
	if (registry.has<model_component>(eid))
	{
		model_component model = registry.get<model_component>(eid);
		model.layers = layers;
		registry.replace<model_component>(eid, model);
		
		// Apply to child layers
		registry.view<parent_component>().each(
		[&](auto entity, auto& component)
		{
			if (component.parent == eid)
				assign_render_layers(registry, entity, layers);
		});
	}
}

void bind_transform(entt::registry& registry, entt::entity source_eid, entt::entity target_eid)
{
	copy_transform_component copy_transform = {target_eid};
	registry.assign_or_replace<copy_transform_component>(source_eid, copy_transform);
}

math::transform<float> get_local_transform(entt::registry& registry, entt::entity eid)
{
	if (registry.has<transform_component>(eid))
	{
		const transform_component& component = registry.get<transform_component>(eid);
		return component.local;
	}
	
	return math::identity_transform<float>;
}

math::transform<float> get_world_transform(entt::registry& registry, entt::entity eid)
{
	if (registry.has<transform_component>(eid))
	{
		const transform_component& component = registry.get<transform_component>(eid);
		return component.world;
	}
	
	return math::identity_transform<float>;
}

void parent(entt::registry& registry, entt::entity child, entt::entity parent)
{
	parent_component component;
	component.parent = parent;
	registry.assign_or_replace<parent_component>(child, component);
}

} // namespace ec

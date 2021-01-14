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

#include "ecs/commands.hpp"
#include "ecs/components/model-component.hpp"
#include "ecs/components/transform-component.hpp"
#include "ecs/components/copy-transform-component.hpp"
#include "ecs/components/snap-component.hpp"
#include "ecs/components/parent-component.hpp"
#include <limits>

namespace ecs {
namespace command {

void translate(ecs::registry& registry, ecs::entity entity, const float3& translation)
{
	if (registry.has<transform_component>(entity))
	{
		transform_component& transform = registry.get<transform_component>(entity);
		transform.local.translation += translation;
	}
}

void move_to(ecs::registry& registry, ecs::entity entity, const float3& position)
{
	if (registry.has<transform_component>(entity))
	{
		transform_component& transform = registry.get<transform_component>(entity);
		transform.local.translation = position;
	}
}

void warp_to(ecs::registry& registry, ecs::entity entity, const float3& position)
{
	if (registry.has<transform_component>(entity))
	{
		transform_component& transform = registry.get<transform_component>(entity);
		transform.local.translation = position;
		transform.warp = true;
	}
}

void set_scale(ecs::registry& registry, ecs::entity entity, const float3& scale)
{
	if (registry.has<transform_component>(entity))
	{
		transform_component& transform = registry.get<transform_component>(entity);
		transform.local.scale = scale;
	}
}

void set_transform(ecs::registry& registry, ecs::entity entity, const math::transform<float>& transform, bool warp)
{
	if (registry.has<transform_component>(entity))
	{
		transform_component& component = registry.get<transform_component>(entity);
		component.local = transform;
		component.warp = warp;
	}
}

void place(ecs::registry& registry, ecs::entity entity, const float2& translation)
{
	snap_component component;
	component.warp = true;
	component.relative = false;
	component.autoremove = true;
	component.ray.origin = {translation[0], 10000.0f, translation[1]};
	component.ray.direction = {0.0f, -1.0f, 0.0f};
	registry.assign_or_replace<snap_component>(entity, component);
}

void assign_render_layers(ecs::registry& registry, ecs::entity entity, unsigned int layers)
{
	if (registry.has<model_component>(entity))
	{
		model_component model = registry.get<model_component>(entity);
		model.layers = layers;
		registry.replace<model_component>(entity, model);
		
		// Apply to child layers
		registry.view<parent_component>().each(
		[&](ecs::entity entity, auto& component)
		{
			if (component.parent == entity)
				assign_render_layers(registry, entity, layers);
		});
	}
}

void bind_transform(ecs::registry& registry, entity source, entity target)
{
	copy_transform_component copy_transform = {target};
	registry.assign_or_replace<copy_transform_component>(source, copy_transform);
}

math::transform<float> get_local_transform(ecs::registry& registry, ecs::entity entity)
{
	if (registry.has<transform_component>(entity))
	{
		const transform_component& component = registry.get<transform_component>(entity);
		return component.local;
	}
	
	return math::identity_transform<float>;
}

math::transform<float> get_world_transform(ecs::registry& registry, ecs::entity entity)
{
	if (registry.has<transform_component>(entity))
	{
		const transform_component& component = registry.get<transform_component>(entity);
		return component.world;
	}
	
	return math::identity_transform<float>;
}

void parent(ecs::registry& registry, entity child, entity parent)
{
	parent_component component;
	component.parent = parent;
	registry.assign_or_replace<parent_component>(child, component);
}

} // namespace command
} // namespace ecs

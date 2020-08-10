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

#include "entity-commands.hpp"
#include "entity/components/model-component.hpp"
#include "entity/components/transform-component.hpp"
#include "entity/components/copy-transform-component.hpp"

namespace ecs {

void move_to(entt::registry& registry, entt::entity entity, const float3& position)
{
	if (registry.has<transform_component>(entity))
	{
		transform_component& transform = registry.get<transform_component>(entity);
		transform.transform.translation = position;
	}
}

void warp_to(entt::registry& registry, entt::entity entity, const float3& position)
{
	if (registry.has<transform_component>(entity))
	{
		transform_component& transform = registry.get<transform_component>(entity);
		transform.warp = true;
	}
}

void assign_render_layers(entt::registry& registry, entt::entity entity, unsigned int layers)
{
	if (registry.has<model_component>(entity))
	{
		model_component model = registry.get<model_component>(entity);
		model.layers = layers;
		registry.replace<model_component>(entity, model);
	}
}

void bind_transform(entt::registry& registry, entt::entity entity, entt::entity target)
{
	copy_transform_component copy_transform = {target};
	registry.assign_or_replace<copy_transform_component>(entity, copy_transform);
}

} // namespace ecs

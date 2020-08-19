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

namespace ec {
	
using namespace ecs;

void translate(entt::registry& registry, entt::entity eid, const float3& translation)
{
	if (registry.has<transform_component>(eid))
	{
		transform_component& transform = registry.get<transform_component>(eid);
		transform.transform.translation += translation;
	}
}

void move_to(entt::registry& registry, entt::entity eid, const float3& position)
{
	if (registry.has<transform_component>(eid))
	{
		transform_component& transform = registry.get<transform_component>(eid);
		transform.transform.translation = position;
	}
}

void warp_to(entt::registry& registry, entt::entity eid, const float3& position)
{
	if (registry.has<transform_component>(eid))
	{
		transform_component& transform = registry.get<transform_component>(eid);
		transform.warp = true;
	}
}

void set_transform(entt::registry& registry, entt::entity eid, const math::transform<float>& transform, bool warp)
{
	if (registry.has<transform_component>(eid))
	{
		transform_component& component = registry.get<transform_component>(eid);
		component.transform = transform;
		component.warp = warp;
	}
}

void assign_render_layers(entt::registry& registry, entt::entity eid, unsigned int layers)
{
	if (registry.has<model_component>(eid))
	{
		model_component model = registry.get<model_component>(eid);
		model.layers = layers;
		registry.replace<model_component>(eid, model);
	}
}

void bind_transform(entt::registry& registry, entt::entity source_eid, entt::entity target_eid)
{
	copy_transform_component copy_transform = {target_eid};
	registry.assign_or_replace<copy_transform_component>(source_eid, copy_transform);
}

} // namespace ec

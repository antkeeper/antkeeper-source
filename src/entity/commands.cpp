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

#include "entity/commands.hpp"
#include "game/component/model.hpp"
#include "game/component/transform.hpp"
#include "game/component/snap.hpp"
#include "game/component/parent.hpp"
#include "game/component/celestial-body.hpp"
#include "game/component/terrain.hpp"
#include <limits>

namespace entity {
namespace command {

void translate(entity::registry& registry, entity::id eid, const float3& translation)
{
	if (registry.has<game::component::transform>(eid))
	{
		game::component::transform& transform = registry.get<game::component::transform>(eid);
		transform.local.translation += translation;
	}
}

void rotate(entity::registry& registry, entity::id eid, float angle, const float3& axis)
{
	if (registry.has<game::component::transform>(eid))
	{
		game::component::transform& transform = registry.get<game::component::transform>(eid);
		transform.local.rotation = math::angle_axis(angle, axis) * transform.local.rotation;
	}
}

void move_to(entity::registry& registry, entity::id eid, const float3& position)
{
	if (registry.has<game::component::transform>(eid))
	{
		game::component::transform& transform = registry.get<game::component::transform>(eid);
		transform.local.translation = position;
	}
}

void warp_to(entity::registry& registry, entity::id eid, const float3& position)
{
	if (registry.has<game::component::transform>(eid))
	{
		game::component::transform& transform = registry.get<game::component::transform>(eid);
		transform.local.translation = position;
		transform.warp = true;
	}
}

void set_scale(entity::registry& registry, entity::id eid, const float3& scale)
{
	if (registry.has<game::component::transform>(eid))
	{
		game::component::transform& transform = registry.get<game::component::transform>(eid);
		transform.local.scale = scale;
	}
}

void set_transform(entity::registry& registry, entity::id eid, const math::transform<float>& transform, bool warp)
{
	if (registry.has<game::component::transform>(eid))
	{
		game::component::transform& component = registry.get<game::component::transform>(eid);
		component.local = transform;
		component.warp = warp;
	}
}

void place(entity::registry& registry, entity::id eid, entity::id celestial_body_id, double altitude, double latitude, double longitude)
{
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
		transform.local.translation = math::type_cast<float>(double3{x, y, z});
		transform.warp = true;
	}
}

void assign_render_layers(entity::registry& registry, entity::id eid, unsigned int layers)
{
	if (registry.has<game::component::model>(eid))
	{
		game::component::model model = registry.get<game::component::model>(eid);
		model.layers = layers;
		registry.replace<game::component::model>(eid, model);
		
		// Apply to child layers
		registry.view<game::component::parent>().each(
		[&](entity::id eid, auto& component)
		{
			if (component.parent == eid)
				assign_render_layers(registry, eid, layers);
		});
	}
}

math::transform<float> get_local_transform(entity::registry& registry, entity::id eid)
{
	if (registry.has<game::component::transform>(eid))
	{
		const game::component::transform& component = registry.get<game::component::transform>(eid);
		return component.local;
	}
	
	return math::transform<float>::identity;
}

math::transform<float> get_world_transform(entity::registry& registry, entity::id eid)
{
	if (registry.has<game::component::transform>(eid))
	{
		const game::component::transform& component = registry.get<game::component::transform>(eid);
		return component.world;
	}
	
	return math::transform<float>::identity;
}

void parent(entity::registry& registry, entity::id child, entity::id parent)
{
	game::component::parent component;
	component.parent = parent;
	registry.assign_or_replace<game::component::parent>(child, component);
}

} // namespace command
} // namespace entity

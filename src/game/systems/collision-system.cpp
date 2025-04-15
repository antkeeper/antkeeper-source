// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <entt/entt.hpp>
#include "game/systems/collision-system.hpp"
#include "game/components/transform-component.hpp"
#include "game/components/picking-component.hpp"
import engine.geom.intersection;
import engine.geom.primitives.plane;
import <limits>;

void collision_system::fixed_update(entity::registry&, float, float)
{}

entity::id collision_system::pick_nearest(const entity::registry& registry, const geom::ray<float, 3>& ray, u32 flags)
{
	entity::id nearest_eid = entt::null;
	float nearest_distance = std::numeric_limits<float>::infinity();
	
	// For each entity with picking and transform components
	registry.view<const picking_component, const transform_component>().each
	(
		[&](entity::id entity_id, const auto& picking, const auto& transform)
		{
			// Skip entity if picking flags don't match
			if (!~(flags | picking.flags))
				return;
			
			// Transform picking sphere
			const geom::sphere<float> sphere =
			{
				transform.world * picking.sphere.center,
				picking.sphere.radius * math::max_element(transform.world.scale)
			};
			
			// Test for intersection between ray and sphere
			auto result = geom::intersection(ray, sphere);
			if (result)
			{
				float t0 = std::get<0>(*result);
				// float t1 = std::get<1>(*result);
				
				if (t0 < nearest_distance)
				{
					nearest_eid = entity_id;
					nearest_distance = t0;
				}
			}
		}
	);
	
	return nearest_eid;
}

entity::id collision_system::pick_nearest(const entity::registry& registry, const math::fvec3& origin, const math::fvec3& normal, u32 flags)
{
	entity::id nearest_eid = entt::null;
	float nearest_sqr_distance = std::numeric_limits<float>::infinity();
	
	// Construct picking plane
	const geom::plane<float> picking_plane = geom::plane<float>(origin, normal);
	
	// For each entity with picking and transform components
	registry.view<const picking_component, const transform_component>().each
	(
		[&](entity::id entity_id, const auto& picking, const auto& transform)
		{
			// Skip entity if picking flags don't match
			if (!~(flags | picking.flags))
				return;
			
			// Transform picking sphere center
			math::fvec3 picking_sphere_center = transform.world * picking.sphere.center;
			
			// Skip entity if picking sphere center has negative distance from picking plane
			if (picking_plane.distance(picking_sphere_center) < 0.0f)
				return;
			
			// Measure distance from picking plane origin to picking sphere center
			const float sqr_distance = math::sqr_distance(picking_sphere_center, origin);
			
			// Check if entity is nearer than the current nearest entity
			if (sqr_distance < nearest_sqr_distance)
			{
				nearest_eid = entity_id;
				nearest_sqr_distance = sqr_distance;
			}
		}
	);
	
	return nearest_eid;
}


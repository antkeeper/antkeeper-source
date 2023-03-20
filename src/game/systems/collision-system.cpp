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

#include "game/systems/collision-system.hpp"
#include "game/components/transform-component.hpp"
#include "game/components/picking-component.hpp"
#include "game/components/collision-component.hpp"
#include "game/components/rigid-body-component.hpp"
#include <engine/geom/primitives/intersection.hpp>
#include <engine/geom/primitives/plane.hpp>
#include <engine/math/transform-operators.hpp>
#include <limits>


collision_system::collision_system(entity::registry& registry):
	updatable_system(registry)
{
	registry.on_construct<collision_component>().connect<&collision_system::on_collision_construct>(this);
	registry.on_update<collision_component>().connect<&collision_system::on_collision_update>(this);
	registry.on_destroy<collision_component>().connect<&collision_system::on_collision_destroy>(this);
}

collision_system::~collision_system()
{
	registry.on_construct<collision_component>().disconnect<&collision_system::on_collision_construct>(this);
	registry.on_update<collision_component>().disconnect<&collision_system::on_collision_update>(this);
	registry.on_destroy<collision_component>().disconnect<&collision_system::on_collision_destroy>(this);
}

void collision_system::update(float t, float dt)
{

}

entity::id collision_system::pick_nearest(const geom::primitive::ray<float, 3>& ray, std::uint32_t flags) const
{
	entity::id nearest_eid = entt::null;
	float nearest_distance = std::numeric_limits<float>::infinity();
	
	// For each entity with picking and transform components
	registry.view<picking_component, transform_component>().each
	(
		[&](entity::id entity_id, const auto& picking, const auto& transform)
		{
			// Skip entity if picking flags don't match
			if (!~(flags | picking.flags))
				return;
			
			// Transform picking sphere
			const geom::primitive::sphere<float> sphere =
			{
				transform.world * picking.sphere.center,
				picking.sphere.radius * math::max(transform.world.scale)
			};
			
			// Test for intersection between ray and sphere
			auto result = geom::primitive::intersection(ray, sphere);
			if (result)
			{
				float t0 = std::get<0>(*result);
				float t1 = std::get<1>(*result);
				
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

entity::id collision_system::pick_nearest(const float3& origin, const float3& normal, std::uint32_t flags) const
{
	entity::id nearest_eid = entt::null;
	float nearest_sqr_distance = std::numeric_limits<float>::infinity();
	
	// Construct picking plane
	const geom::primitive::plane<float> picking_plane = geom::primitive::plane<float>(origin, normal);
	
	// For each entity with picking and transform components
	registry.view<picking_component, transform_component>().each
	(
		[&](entity::id entity_id, const auto& picking, const auto& transform)
		{
			// Skip entity if picking flags don't match
			if (!~(flags | picking.flags))
				return;
			
			// Transform picking sphere center
			float3 picking_sphere_center = transform.world * picking.sphere.center;
			
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

void collision_system::on_collision_construct(entity::registry& registry, entity::id entity_id)
{}

void collision_system::on_collision_update(entity::registry& registry, entity::id entity_id)
{}

void collision_system::on_collision_destroy(entity::registry& registry, entity::id entity_id)
{}


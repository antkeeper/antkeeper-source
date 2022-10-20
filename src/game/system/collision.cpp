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

#include "collision.hpp"
#include "game/component/transform.hpp"
#include "game/component/picking.hpp"
#include "geom/primitive/intersection.hpp"
#include "geom/primitive/plane.hpp"
#include <limits>

namespace game {
namespace system {

collision::collision(entity::registry& registry):
	updatable(registry)
{
	registry.on_construct<component::collision>().connect<&collision::on_collision_construct>(this);
	registry.on_update<component::collision>().connect<&collision::on_collision_update>(this);
	registry.on_destroy<component::collision>().connect<&collision::on_collision_destroy>(this);
}

void collision::update(double t, double dt)
{
	registry.on_construct<component::collision>().disconnect<&collision::on_collision_construct>(this);
	registry.on_update<component::collision>().disconnect<&collision::on_collision_update>(this);
	registry.on_destroy<component::collision>().disconnect<&collision::on_collision_destroy>(this);
}

entity::id collision::pick_nearest(const geom::primitive::ray<float, 3>& ray, std::uint32_t flags) const
{
	entity::id nearest_eid = entt::null;
	float nearest_distance = std::numeric_limits<float>::infinity();
	
	// For each entity with picking and transform components
	registry.view<component::picking, component::transform>().each
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
				picking.sphere.radius * std::max(std::max(transform.world.scale[0], transform.world.scale[1]), transform.world.scale[2])
			};
			
			// Test for intersection between ray and sphere
			auto result = geom::primitive::intersection(ray, sphere);
			if (result && std::get<0>(*result) < nearest_distance)
			{
				nearest_eid = entity_id;
				nearest_distance = std::get<0>(*result);
			}
		}
	);
	
	return nearest_eid;
}

entity::id collision::pick_nearest(const float3& origin, const float3& normal, std::uint32_t flags) const
{
	entity::id nearest_eid = entt::null;
	float nearest_distance_squared = std::numeric_limits<float>::infinity();
	
	// Construct picking plane
	const geom::primitive::plane<float> picking_plane = geom::primitive::plane<float>(origin, normal);
	
	// For each entity with picking and transform components
	registry.view<component::picking, component::transform>().each
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
			const float distance_squared = math::distance_squared(picking_sphere_center, origin);
			
			// Check if entity is nearer than the current nearest entity
			if (distance_squared < nearest_distance_squared)
			{
				nearest_eid = entity_id;
				nearest_distance_squared = distance_squared;
			}
		}
	);
	
	return nearest_eid;
}

void collision::on_collision_construct(entity::registry& registry, entity::id entity_id)
{}

void collision::on_collision_update(entity::registry& registry, entity::id entity_id)
{}

void collision::on_collision_destroy(entity::registry& registry, entity::id entity_id)
{}

} // namespace system
} // namespace game

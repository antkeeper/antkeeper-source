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

#ifndef ANTKEEPER_GAME_COLLISION_SYSTEM_HPP
#define ANTKEEPER_GAME_COLLISION_SYSTEM_HPP

#include "game/systems/updatable-system.hpp"
#include <engine/entity/id.hpp>
#include "game/components/collision-component.hpp"
#include <engine/geom/primitive/ray.hpp>


/**
 * Maintains a spatially partitioned set of collision meshes.
 */
class collision_system: public updatable_system
{
public:
	explicit collision_system(entity::registry& registry);
	virtual void update(float t, float dt);
	
	/**
	 * Picks the nearest entity with the specified picking flags that intersects a ray.
	 *
	 * @param ray Picking ray.
	 * @param flags Picking flags.
	 *
	 * @return ID of the picked entity, or `entt::null` if no entity was picked.
	 */
	entity::id pick_nearest(const geom::primitive::ray<float, 3>& ray, std::uint32_t flags) const;
	
	/**
	 * Picks the nearest entity with the specified picking flags that has a non-negative distance from a plane.
	 *
	 * @param origin Origin of the picking plane.
	 * @param normal Picking plane normal direction.
	 * @param flags Picking flags.
	 *
	 * @return ID of the picked entity, or `entt::null` if no entity was picked.
	 */
	entity::id pick_nearest(const float3& origin, const float3& normal, std::uint32_t flags) const;

private:	
	void on_collision_construct(entity::registry& registry, entity::id entity_id);
	void on_collision_update(entity::registry& registry, entity::id entity_id);
	void on_collision_destroy(entity::registry& registry, entity::id entity_id);
};


#endif // ANTKEEPER_GAME_COLLISION_SYSTEM_HPP


// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_COLLISION_SYSTEM_HPP
#define ANTKEEPER_GAME_COLLISION_SYSTEM_HPP

#include "game/systems/updatable-system.hpp"
#include <engine/entity/id.hpp>
#include <engine/geom/primitives/ray.hpp>


/**
 * Maintains a spatially partitioned set of collision meshes.
 */
class collision_system: public updatable_system
{
public:
	explicit collision_system(entity::registry& registry);
	~collision_system();
	
	virtual void update(float t, float dt);
	
	/**
	 * Picks the nearest entity with the specified picking flags that intersects a ray.
	 *
	 * @param ray Picking ray.
	 * @param flags Picking flags.
	 *
	 * @return ID of the picked entity, or `entt::null` if no entity was picked.
	 */
	entity::id pick_nearest(const geom::ray<float, 3>& ray, std::uint32_t flags) const;
	
	/**
	 * Picks the nearest entity with the specified picking flags that has a non-negative distance from a plane.
	 *
	 * @param origin Origin of the picking plane.
	 * @param normal Picking plane normal direction.
	 * @param flags Picking flags.
	 *
	 * @return ID of the picked entity, or `entt::null` if no entity was picked.
	 */
	entity::id pick_nearest(const math::fvec3& origin, const math::fvec3& normal, std::uint32_t flags) const;
};


#endif // ANTKEEPER_GAME_COLLISION_SYSTEM_HPP


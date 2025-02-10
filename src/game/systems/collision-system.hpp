// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_COLLISION_SYSTEM_HPP
#define ANTKEEPER_GAME_COLLISION_SYSTEM_HPP

#include "game/systems/fixed-update-system.hpp"
#include <engine/entity/id.hpp>
#include <engine/geom/primitives/ray.hpp>

/// Maintains a spatially partitioned set of collision meshes.
class collision_system: public fixed_update_system
{
public:
	~collision_system() override = default;
	void fixed_update(entity::registry& registry, float t, float dt) override;
	
	/// Picks the nearest entity with the specified picking flags that intersects a ray.
	/// @param ray Picking ray.
	/// @param flags Picking flags.
	/// @return ID of the picked entity, or `entt::null` if no entity was picked.
	[[nodiscard]] static entity::id pick_nearest(const entity::registry& registry, const geom::ray<float, 3>& ray, std::uint32_t flags);
	
	/// Picks the nearest entity with the specified picking flags that has a non-negative distance from a plane.
	/// @param origin Origin of the picking plane.
	/// @param normal Picking plane normal direction.
	/// @param flags Picking flags.
	/// @return ID of the picked entity, or `entt::null` if no entity was picked.
	[[nodiscard]] static entity::id pick_nearest(const entity::registry& registry, const math::fvec3& origin, const math::fvec3& normal, std::uint32_t flags);
};


#endif // ANTKEEPER_GAME_COLLISION_SYSTEM_HPP


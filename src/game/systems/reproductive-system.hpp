// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_REPRODUCTIVE_SYSTEM_HPP
#define ANTKEEPER_GAME_REPRODUCTIVE_SYSTEM_HPP

#include "game/systems/updatable-system.hpp"

class physics_system;

/**
 * 
 */
class reproductive_system:
	public updatable_system
{
public:
	explicit reproductive_system(entity::registry& registry);
	~reproductive_system() override = default;
	
	void update(float t, float dt) override;
	
	/**
	 * Sets the factor by which the timestep `dt` will be scaled.
	 *
	 * @param scale Factor by which to scale the timestep.
	 */
	inline constexpr void set_time_scale(float scale) noexcept
	{
		m_time_scale = scale;
	}
	
	inline constexpr void set_physics_system(physics_system* physics_system) noexcept
	{
		m_physics_system = physics_system;
	}
	
private:
	float m_time_scale{1.0f};
	physics_system* m_physics_system{};
};

#endif // ANTKEEPER_GAME_REPRODUCTIVE_SYSTEM_HPP

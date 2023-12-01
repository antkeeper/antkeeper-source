// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_METABOLIC_SYSTEM_HPP
#define ANTKEEPER_GAME_METABOLIC_SYSTEM_HPP

#include "game/systems/updatable-system.hpp"

/**
 * 
 */
class metabolic_system:
	public updatable_system
{
public:
	explicit metabolic_system(entity::registry& registry);
	virtual void update(float t, float dt);
	
	/**
	 * Sets the factor by which the timestep `dt` will be scaled.
	 *
	 * @param scale Factor by which to scale the timestep.
	 */
	inline constexpr void set_time_scale(float scale) noexcept
	{
		m_time_scale = scale;
	}
	
private:
	float m_time_scale{1.0f};
};

#endif // ANTKEEPER_GAME_METABOLIC_SYSTEM_HPP

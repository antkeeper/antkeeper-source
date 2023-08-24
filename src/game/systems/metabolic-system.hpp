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

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

#ifndef ANTKEEPER_SCENE_AMBIENT_LIGHT_HPP
#define ANTKEEPER_SCENE_AMBIENT_LIGHT_HPP

#include <engine/scene/light.hpp>
#include <engine/math/vector.hpp>

namespace scene {

/**
 * Omnidirectional source of illuminance.
 */
class ambient_light: public light
{
public:
	[[nodiscard]] inline light_type get_light_type() const noexcept override
	{
		return light_type::ambient;
	}
	
	/**
	 * Sets the illuminance of the ambient light.
	 *
	 * @param illuminance Illuminance, in *lx*.
	 */
	inline void set_illuminance(const math::vector<float, 3>& illuminance) noexcept
	{
		m_illuminance = illuminance;
	}
	
	/// Returns the illuminance of the ambient light, in *lx*.
	[[nodiscard]] inline const math::vector<float, 3>& get_illuminance() const noexcept
	{
		return m_illuminance;
	}
	
private:
	math::vector<float, 3> m_illuminance{0.0f, 0.0f, 0.0f};
};

} // namespace scene

#endif // ANTKEEPER_SCENE_AMBIENT_LIGHT_HPP

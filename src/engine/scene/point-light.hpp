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

#ifndef ANTKEEPER_SCENE_POINT_LIGHT_HPP
#define ANTKEEPER_SCENE_POINT_LIGHT_HPP

#include <engine/scene/light.hpp>
#include <engine/math/vector.hpp>

namespace scene {

/**
 * Light source that radiates outward from a point.
 */
class point_light: public light
{
public:
	/// Returns light_type::point
	[[nodiscard]] inline light_type get_light_type() const noexcept override
	{
		return light_type::point;
	}
	
	/**
	 * Sets the luminous flux of the point light.
	 *
	 * @param luminous_flux Luminous flux, in *lm*.
	 */
	inline void set_luminous_flux(const math::vector<float, 3>& luminous_flux) noexcept
	{
		m_luminous_flux = luminous_flux;
	}
	
	/// Returns the luminous flux of the point light, in *lm*.
	[[nodiscard]] inline const math::vector<float, 3>& get_luminous_flux() const noexcept
	{
		return m_luminous_flux;
	}
	
	/**
	 * Sets the attenuation factors of the light.
	 *
	 * @param attenuation Vector containing the constant, linear, and quadratic attenuation factors, as x, y, and z, respectively.
	 */
	inline void set_attenuation(const float3& attenuation) noexcept
	{
		m_attenuation = attenuation;
	}

	/// Returns the attenuation factors of the light.
	[[nodiscard]] inline const math::vector<float, 3>& get_attenuation() const noexcept
	{
		return m_attenuation;
	}

private:
	math::vector<float, 3> m_luminous_flux{0.0f, 0.0f, 0.0f};
	math::vector<float, 3> m_attenuation{1.0f, 0.0f, 0.0f};
};

} // namespace scene

#endif // ANTKEEPER_SCENE_POINT_LIGHT_HPP

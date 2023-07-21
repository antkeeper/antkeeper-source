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

#ifndef ANTKEEPER_SCENE_SPOT_LIGHT_HPP
#define ANTKEEPER_SCENE_SPOT_LIGHT_HPP

#include <engine/scene/light.hpp>
#include <engine/math/numbers.hpp>
#include <engine/math/vector.hpp>

namespace scene {

/**
 * Directional cone light source.
 */
class spot_light: public light
{
public:
	/// Returns light_type::spot.
	[[nodiscard]] inline light_type get_light_type() const noexcept override
	{
		return light_type::spot;
	}
	
	/**
	 * Sets the luminous flux of the spot light.
	 *
	 * @param luminous_flux Luminous flux, in *lm*.
	 */
	inline void set_luminous_flux(const math::fvec3& luminous_flux) noexcept
	{
		m_luminous_flux = luminous_flux;
	}
	
	/// Returns the luminous flux of the spot light, in *lm*.
	[[nodiscard]] inline const math::fvec3& get_luminous_flux() const noexcept
	{
		return m_luminous_flux;
	}
	
	/**
	 * Sets the spot light cutoff angles.
	 *
	 * @param cutoff Vector containing the inner and outer cutoff angles, as x and y, respectively.
	 */
	void set_cutoff(const math::fvec2& cutoff);
	
	/// Returns the direction vector.
	[[nodiscard]] inline const math::fvec3& get_direction() const noexcept
	{
		return m_direction;
	}
	
	/// Returns the spot light cutoff angles.
	[[nodiscard]] inline const math::fvec2& get_cutoff() const noexcept
	{
		return m_cutoff;
	}
	
	/// Returns the cosine of the spot light cutoff angles.
	[[nodiscard]] inline const math::fvec2& get_cosine_cutoff() const noexcept
	{
		return m_cosine_cutoff;
	}

private:
	void transformed() override;
	
	math::fvec3 m_luminous_flux{0.0f, 0.0f, 0.0f};
	math::fvec3 m_direction{0.0f, 0.0f, -1.0f};
	math::fvec2 m_cutoff{math::pi<float>, math::pi<float>};
	math::fvec2 m_cosine_cutoff{-1.0f, -1.0f};
};

} // namespace scene

#endif // ANTKEEPER_SCENE_SPOT_LIGHT_HPP


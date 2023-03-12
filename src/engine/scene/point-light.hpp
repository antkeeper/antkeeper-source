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
#include <engine/utility/fundamental-types.hpp>

namespace scene {

/**
 * Light source that radiates outward from a point.
 */
class point_light: public light
{
public:
	point_light();
	
	/// Returns light_type::point
	[[nodiscard]] inline light_type get_light_type() const noexcept override
	{
		return light_type::point;
	}
	
	/**
	 * Sets the attenuation factors of the light.
	 *
	 * @param attenuation Vector containing the constant, linear, and quadratic attenuation factors, as x, y, and z, respectively.
	 */
	void set_attenuation(const float3& attenuation);

	/// Returns the attenuation factors of the light.
	[[nodiscard]] inline const float3& get_attenuation() const noexcept
	{
		return attenuation[1];
	}

	/// Returns the attenuation tween.
	[[nodiscard]] inline const tween<float3>& get_attenuation_tween() const noexcept
	{
		return attenuation;
	}

	void update_tweens() override;

private:
	tween<float3> attenuation;
};

} // namespace scene

#endif // ANTKEEPER_SCENE_POINT_LIGHT_HPP


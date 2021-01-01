/*
 * Copyright (C) 2021  Christopher J. Howard
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

#ifndef ANTKEEPER_POINT_LIGHT_HPP
#define ANTKEEPER_POINT_LIGHT_HPP

#include "scene/light.hpp"
#include "utility/fundamental-types.hpp"

class point_light: public light
{
public:
	point_light();
	
	/// Returns light_type::point
	virtual light_type get_light_type() const;
	
	/**
	 * Sets the attenuation factors of the light.
	 *
	 * @param attenuation Vector containing the constant, linear, and quadratic attenuation factors, as x, y, and z, respectively.
	 */
	void set_attenuation(const float3& attenuation);

	/// Returns the attenuation factors of the light.
	const float3& get_attenuation() const;

	/// Returns the attenuation tween.
	const tween<float3>& get_attenuation_tween() const;

	/// @copydoc scene_object_base::update_tweens();
	virtual void update_tweens();

private:
	tween<float3> attenuation;
};

inline light_type point_light::get_light_type() const
{
	return light_type::point;
}

inline const float3& point_light::get_attenuation() const
{
	return attenuation[1];
}

inline const tween<float3>& point_light::get_attenuation_tween() const
{
	return attenuation;
}

#endif // ANTKEEPER_POINT_LIGHT_HPP


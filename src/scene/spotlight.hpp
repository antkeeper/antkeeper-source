/*
 * Copyright (C) 2020  Christopher J. Howard
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

#ifndef ANTKEEPER_SPOTLIGHT_HPP
#define ANTKEEPER_SPOTLIGHT_HPP

#include "scene/light.hpp"
#include "utility/fundamental-types.hpp"

class spotlight: public light
{
public:
	spotlight();
	
	/// Returns light_type::spot
	virtual light_type get_light_type() const;
	
	/**
	 * Sets the attenuation factors of the light.
	 *
	 * @param attenuation Vector containing the constant, linear, and quadratic attenuation factors, as x, y, and z, respectively.
	 */
	void set_attenuation(const float3& attenuation);
	
	/**
	 * Sets the spotlight cutoff angles.
	 *
	 * @param cutoff Vector containing the inner and outer cutoff angles, as x and y, respectively.
	 */
	void set_cutoff(const float2& cutoff);
	
	/// Returns the direction vector.
	const float3& get_direction() const;
	
	/// Returns the attenuation factors of the light.
	const float3& get_attenuation() const;
	
	/// Returns the spotlight cutoff angles.
	const float2& get_cutoff() const;
	
	/// Returns the cosine of the spotlight cutoff angles.
	const float2& get_cosine_cutoff() const;
	
	/// Returns the direction tween.
	const tween<float3>& get_direction_tween() const;
	
	/// Returns the attenuation tween.
	const tween<float3>& get_attenuation_tween() const;
	
	/// Returns the cutoff tween.
	const tween<float2>& get_cutoff_tween() const;
	
	/// Returns the cosine cutoff tween.
	const tween<float2>& get_cosine_cutoff_tween() const;

	/// @copydoc scene_object_base::update_tweens();
	virtual void update_tweens();

private:
	virtual void transformed();

	tween<float3> direction;
	tween<float3> attenuation;
	tween<float2> cutoff;
	tween<float2> cosine_cutoff;
};

inline light_type spotlight::get_light_type() const
{
	return light_type::spot;
}

inline const float3& spotlight::get_direction() const
{
	return direction[1];
}

inline const float3& spotlight::get_attenuation() const
{
	return attenuation[1];
}

inline const float2& spotlight::get_cutoff() const
{
	return cutoff[1];
}

inline const float2& spotlight::get_cosine_cutoff() const
{
	return cosine_cutoff[1];
}

inline const tween<float3>& spotlight::get_direction_tween() const
{
	return direction;
}

inline const tween<float3>& spotlight::get_attenuation_tween() const
{
	return attenuation;
}

inline const tween<float2>& spotlight::get_cutoff_tween() const
{
	return cutoff;
}

inline const tween<float2>& spotlight::get_cosine_cutoff_tween() const
{
	return cosine_cutoff;
}

#endif // ANTKEEPER_SPOTLIGHT_HPP


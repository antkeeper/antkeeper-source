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
#include <engine/utility/fundamental-types.hpp>

namespace scene {

/**
 * Directional cone light source.
 */
class spot_light: public light
{
public:
	/// Creates a spot light.
	spot_light();
	
	/// Returns light_type::spot
	[[nodiscard]] inline light_type get_light_type() const noexcept override
	{
		return light_type::spot;
	}
	
	/**
	 * Sets the attenuation factors of the light.
	 *
	 * @param attenuation Vector containing the constant, linear, and quadratic attenuation factors, as x, y, and z, respectively.
	 */
	void set_attenuation(const float3& attenuation);
	
	/**
	 * Sets the spot light cutoff angles.
	 *
	 * @param cutoff Vector containing the inner and outer cutoff angles, as x and y, respectively.
	 */
	void set_cutoff(const float2& cutoff);
	
	/// Returns the direction vector.
	[[nodiscard]] inline const float3& get_direction() const noexcept
	{
		return direction[1];
	}
	
	/// Returns the attenuation factors of the light.
	[[nodiscard]] inline const float3& get_attenuation() const noexcept
	{
		return attenuation[1];
	}
	
	/// Returns the spot light cutoff angles.
	[[nodiscard]] inline const float2& get_cutoff() const noexcept
	{
		return cutoff[1];
	}
	
	/// Returns the cosine of the spot light cutoff angles.
	[[nodiscard]] inline const float2& get_cosine_cutoff() const noexcept
	{
		return cosine_cutoff[1];
	}
	
	/// Returns the direction tween.
	[[nodiscard]] inline const tween<float3>& get_direction_tween() const noexcept
	{
		return direction;
	}
	
	/// Returns the attenuation tween.
	[[nodiscard]] inline const tween<float3>& get_attenuation_tween() const noexcept
	{
		return attenuation;
	}
	
	/// Returns the cutoff tween.
	[[nodiscard]] inline const tween<float2>& get_cutoff_tween() const noexcept
	{
		return cutoff;
	}
	
	/// Returns the cosine cutoff tween.
	[[nodiscard]] inline const tween<float2>& get_cosine_cutoff_tween() const noexcept
	{
		return cosine_cutoff;
	}

	void update_tweens() override;

private:
	void transformed() override;

	tween<float3> direction;
	tween<float3> attenuation;
	tween<float2> cutoff;
	tween<float2> cosine_cutoff;
};

} // namespace scene

#endif // ANTKEEPER_SCENE_SPOT_LIGHT_HPP


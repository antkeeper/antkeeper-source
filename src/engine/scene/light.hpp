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

#ifndef ANTKEEPER_SCENE_LIGHT_HPP
#define ANTKEEPER_SCENE_LIGHT_HPP

#include <engine/scene/object.hpp>
#include <engine/geom/sphere.hpp>
#include <engine/utility/fundamental-types.hpp>

namespace scene {

/// Light object type enumerations.
enum class light_type
{
	/// Denotes an ambient light.
	ambient,
	
	/// Denotes a directional light.
	directional,
	
	/// Denotes a point light.
	point,
	
	/// Denotes a spot light.
	spot
};

/**
 * Abstract base class for light objects.
 */
class light: public object<light>
{
public:
	typedef geom::sphere<float> sphere_type;
	
	/// Creates a light.
	light();

	/// Returns an enumeration denoting the light object type.
	[[nodiscard]] virtual light_type get_light_type() const noexcept = 0;
	
	/**
	 * Sets the color of the light.
	 *
	 * @param color Scene-linear light color.
	 */
	void set_color(const float3& color);
	
	/**
	 * Sets the intensity of the light.
	 *
	 * @param intensity Light intensity.
	 */
	void set_intensity(float intensity);
	
	/// Returns the local-space bounding volume of the light.
	inline const bounding_volume_type& get_local_bounds() const noexcept override
	{
		return local_bounds;
	}
	
	/// Returns the world-space bounding volume of the light.
	inline const bounding_volume_type& get_world_bounds() const noexcept override
	{
		return world_bounds;
	}
	
	/// Returns the light color.
	[[nodiscard]] inline const float3& get_color() const noexcept
	{
		return color[1];
	}
	
	/// Returns the light intensity.
	[[nodiscard]] inline float get_intensity() const noexcept
	{
		return intensity[1];
	}
	
	/// Returns the intensity-scaled light color.
	[[nodiscard]] inline const float3& get_scaled_color() const noexcept
	{
		return scaled_color[1];
	}

	[[nodiscard]] inline const tween<float3>& get_color_tween() const noexcept
	{
		return color;
	}
	
	[[nodiscard]] inline const tween<float>& get_intensity_tween() const noexcept
	{
		return intensity;
	}
	
	[[nodiscard]] inline const tween<float3>& get_scaled_color_tween() const noexcept
	{
		return scaled_color;
	}

	/// @copydoc object_base::update_tweens();
	virtual void update_tweens();

private:
	virtual void transformed();
	
	tween<float3> color;
	tween<float> intensity;
	tween<float3> scaled_color;
	sphere_type local_bounds;
	sphere_type world_bounds;
};

} // namespace scene

#endif // ANTKEEPER_SCENE_LIGHT_HPP

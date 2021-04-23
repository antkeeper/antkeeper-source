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

#ifndef ANTKEEPER_SCENE_LIGHT_HPP
#define ANTKEEPER_SCENE_LIGHT_HPP

#include "scene/object.hpp"
#include "geom/sphere.hpp"
#include "utility/fundamental-types.hpp"

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
	virtual light_type get_light_type() const = 0;
	
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
	
	/// Returns the bounding volume of the light.
	virtual const bounding_volume_type& get_bounds() const;
	
	/// Returns the light color.
	const float3& get_color() const;
	
	/// Returns the light intensity.
	float get_intensity() const;
	
	/// Returns the intensity-scaled light color.
	const float3& get_scaled_color() const;

	const tween<float3>& get_color_tween() const;
	const tween<float>& get_intensity_tween() const;
	const tween<float3>& get_scaled_color_tween() const;

	/// @copydoc object_base::update_tweens();
	virtual void update_tweens();

private:
	virtual void transformed();
	
	tween<float3> color;
	tween<float> intensity;
	tween<float3> scaled_color;
	sphere_type bounds;
};

inline const typename object_base::bounding_volume_type& light::get_bounds() const
{
	return bounds;
}

inline const float3& light::get_color() const
{
	return color[1];
}

inline float light::get_intensity() const
{
	return intensity[1];
}

inline const float3& light::get_scaled_color() const
{
	return scaled_color[1];
}

inline const tween<float3>& light::get_color_tween() const
{
	return color;
}

inline const tween<float>& light::get_intensity_tween() const
{
	return intensity;
}

inline const tween<float3>& light::get_scaled_color_tween() const
{
	return scaled_color;
}

} // namespace scene

#endif // ANTKEEPER_SCENE_LIGHT_HPP


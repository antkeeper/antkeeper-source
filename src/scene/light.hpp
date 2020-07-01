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

#ifndef ANTKEEPER_LIGHT_HPP
#define ANTKEEPER_LIGHT_HPP

#include "scene/scene-object.hpp"
#include "geometry/sphere.hpp"

enum class light_type
{
	ambient,
	directional,
	point,
	spot
};

class light: public scene_object<light>
{
public:
	light();

	virtual light_type get_light_type() const = 0;

	void set_color(const float3& color);
	void set_intensity(float intensity);
	
	virtual const bounding_volume<float>& get_bounds() const;

	const float3& get_color() const;
	float get_intensity() const;
	const float3& get_scaled_color() const;

	const tween<float3>& get_color_tween() const;
	const tween<float>& get_intensity_tween() const;
	const tween<float3>& get_scaled_color_tween() const;

	/// @copydoc scene_object_base::update_tweens();
	virtual void update_tweens();

private:
	virtual void transformed();
	
	tween<float3> color;
	tween<float> intensity;
	tween<float3> scaled_color;
	sphere<float> bounds;
};

inline const bounding_volume<float>& light::get_bounds() const
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

#endif // ANTKEEPER_LIGHT_HPP


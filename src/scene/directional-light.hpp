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

#ifndef ANTKEEPER_SCENE_DIRECTIONAL_LIGHT_HPP
#define ANTKEEPER_SCENE_DIRECTIONAL_LIGHT_HPP

#include "scene/light.hpp"
#include "gl/texture-2d.hpp"
#include "utility/fundamental-types.hpp"

namespace scene {

/**
 * Light source with parallel rays and constant intensity.
 */
class directional_light: public light
{
public:
	/// Creates a directional light.
	directional_light();
	
	/**
	 * Sets the light texture, also known as a gobo, cucoloris, or cookie.
	 *
	 * @param texture Light texture.
	 */
	void set_light_texture(const gl::texture_2d* texture);
	
	/**
	 * Sets the opacity of the light texture.
	 *
	 * @param opacity Light texture opacity, on `[0, 1]`.
	 */
	void set_light_texture_opacity(float opacity);
	
	/**
	 * Sets the scale of the light texture.
	 *
	 * @param scale Scale of the light texture.
	 */
	void set_light_texture_scale(const float2& scale);
	
	/// Returns light_type::directional.
	virtual light_type get_light_type() const;

	/// Returns the normalized direction vector of the light.
	const float3& get_direction() const;
	
	/// Returns the light texture for this light, or `nullptr` if no light texture is set.
	const gl::texture_2d* get_light_texture() const;
	
	/// Returns the light texture opacity.
	float get_light_texture_opacity() const;
	
	/// Returns the light texture scale.
	const float2& get_light_texture_scale() const;
	
	/// Returns the light direction tween.
	const tween<float3>& get_direction_tween() const;
	
	/// Returns the light texture opacity tween.
	const tween<float>& get_light_texture_opacity_tween() const;
	
	/// Returns the light texture scale tween.
	const tween<float2>& get_light_texture_scale_tween() const;

	/// @copydoc object_base::update_tweens();
	virtual void update_tweens();

private:
	virtual void transformed();

	tween<float3> direction;
	const gl::texture_2d* light_texture;
	tween<float> light_texture_opacity;
	tween<float2> light_texture_scale;
};

inline light_type directional_light::get_light_type() const
{
	return light_type::directional;
}

inline const float3& directional_light::get_direction() const
{
	return direction[1];
}

inline const gl::texture_2d* directional_light::get_light_texture() const
{
	return light_texture;
}

inline float directional_light::get_light_texture_opacity() const
{
	return light_texture_opacity[1];
}

inline const float2& directional_light::get_light_texture_scale() const
{
	return light_texture_scale[1];
}

inline const tween<float3>& directional_light::get_direction_tween() const
{
	return direction;
}

inline const tween<float>& directional_light::get_light_texture_opacity_tween() const
{
	return light_texture_opacity;
}

inline const tween<float2>& directional_light::get_light_texture_scale_tween() const
{
	return light_texture_scale;
}

} // namespace scene

#endif // ANTKEEPER_SCENE_DIRECTIONAL_LIGHT_HPP

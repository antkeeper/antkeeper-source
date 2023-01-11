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
#include <vector>

namespace scene {

/**
 * Light source with parallel rays and constant intensity.
 */
class directional_light: public light
{
public:
	/// Creates a directional light.
	directional_light();
	
	/// Returns light_type::directional.
	virtual light_type get_light_type() const;

	/// Returns the normalized direction vector of the light.
	const float3& get_direction() const;
	
	/// @copydoc object_base::update_tweens();
	virtual void update_tweens();
	
	/// @name Shadow
	/// @{
	
	/**
	 * Enables or disables shadow casting.
	 *
	 * @param caster `true` if the light should cast shadows, `false` otherwise.
	 */
	void set_shadow_caster(bool caster) noexcept;
	
	/**
	 * Sets the shadow map framebuffer.
	 *
	 * @param framebuffer Pointer to a shadow map framebuffer.
	 */
	void set_shadow_framebuffer(const gl::framebuffer* framebuffer) noexcept;
	
	/**
	 * Sets the shadow bias factor for reducing self-shadowing.
	 *
	 * @param bias Shadow bias factor.
	 */
	void set_shadow_bias(float bias) noexcept;
	
	/**
	 * Sets the number of shadow cascades.
	 *
	 * @param count Number of shadow cascades.
	 */
	void set_shadow_cascade_count(unsigned int count) noexcept;
	
	/**
	 * Sets the shadow cascade coverage factor.
	 *
	 * @param factor Percentage of the view frustum clipping range covered by shadow cascades. A value of `1.0` results in full coverage of the view frustum clipping range, `0.5` results in coverage of half of the clipping range, etc.
	 */
	void set_shadow_cascade_coverage(float factor) noexcept;
	
	/**
	 * Sets the shadow cascade distribution.
	 *
	 * @param weight Linear interpolation weight between uniform and logarithmic cascade distributions. A weight of `0.0` results in a uniform cascade distribution, while `1.0` results in a logarithmic distribution.
	 */
	void set_shadow_cascade_distribution(float weight) noexcept;
	
	/// Returns `true` if the light casts shadows, `false` otherwise.
	bool is_shadow_caster() const noexcept;
	
	/// Returns the shadow map framebuffer, of `nullptr` if no shadow map framebuffer is set.
	const gl::framebuffer* get_shadow_framebuffer() const noexcept;
	
	/// Returns the shadow bias factor.
	float get_shadow_bias() const noexcept;
	
	/// Returns the number of shadow cascades.
	unsigned int get_shadow_cascade_count() const noexcept;
	
	/// Returns the shadow cascade coverage factor.
	float get_shadow_cascade_coverage() const noexcept;
	
	/// Returns the shadow cascade distribution weight.
	float get_shadow_cascade_distribution() const noexcept;
	
	/// Returns the array of shadow cascade far clipping plane distances.
	float* get_shadow_cascade_distances() const noexcept;
	
	/// Returns the array of world-space to cascade texture-space transformation matrices.
	float4x4* get_shadow_cascade_matrices() const noexcept;
	
	/// @}

	/// @name Light texture
	/// @{
	
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
	
	/// @}

private:
	virtual void transformed();

	tween<float3> direction;
	
	bool shadow_caster;
	const gl::framebuffer* shadow_framebuffer;
	float shadow_bias;
	unsigned int shadow_cascade_count;
	float shadow_cascade_coverage;
	float shadow_cascade_distribution;
	mutable std::vector<float> shadow_cascade_distances;
	mutable std::vector<float4x4> shadow_cascade_matrices;
	
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

inline bool directional_light::is_shadow_caster() const noexcept
{
	return shadow_caster;
}

inline const gl::framebuffer* directional_light::get_shadow_framebuffer() const noexcept
{
	return shadow_framebuffer;
}

inline float directional_light::get_shadow_bias() const noexcept
{
	return shadow_bias;
}

inline unsigned int directional_light::get_shadow_cascade_count() const noexcept
{
	return shadow_cascade_count;
}

inline float directional_light::get_shadow_cascade_coverage() const noexcept
{
	return shadow_cascade_coverage;
}

inline float directional_light::get_shadow_cascade_distribution() const noexcept
{
	return shadow_cascade_distribution;
}

inline float* directional_light::get_shadow_cascade_distances() const noexcept
{
	return shadow_cascade_distances.data();
}
	
inline float4x4* directional_light::get_shadow_cascade_matrices() const noexcept
{
	return shadow_cascade_matrices.data();
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

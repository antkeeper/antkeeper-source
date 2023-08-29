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

#include <engine/scene/light.hpp>
#include <engine/gl/texture-2d.hpp>
#include <engine/math/vector.hpp>
#include <memory>
#include <vector>
#include <span>

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
	[[nodiscard]] inline light_type get_light_type() const noexcept override
	{
		return light_type::directional;
	}
	
	/// @name Light
	/// @{
	
	/**
	 * Sets the direction of the directional light.
	 *
	 * @param direction Unit-length light direction vector.
	 */
	void set_direction(const math::fvec3& direction);
	
	/**
	 * Sets the color of the light.
	 *
	 * @param color Light color.
	 */
	inline void set_color(const math::fvec3& color) noexcept
	{
		m_color = color;
		color_updated();
	}
	
	/**
	 * Sets the illuminance of the light on a surface perpendicular to the light direction.
	 *
	 * @param illuminance Illuminance on a surface perpendicular to the light direction.
	 */
	inline void set_illuminance(float illuminance) noexcept
	{
		m_illuminance = illuminance;
		illuminance_updated();
	}
	
	/// Returns a unit vector pointing in the light direction.
	[[nodiscard]] inline constexpr const math::fvec3& get_direction() const noexcept
	{
		return m_direction;
	}
	
	/// Returns the color of the light.
	[[nodiscard]] inline constexpr const math::fvec3& get_color() const noexcept
	{
		return m_color;
	}
	
	/// Returns the illuminance of the light on a surface perpendicular to the light direction.
	[[nodiscard]] inline constexpr float get_illuminance() const noexcept
	{
		return m_illuminance;
	}
	
	/// Returns the color-modulated illuminance of the light on a surface perpendicular to the light direction.
	[[nodiscard]] inline constexpr const math::fvec3& get_colored_illuminance() const noexcept
	{
		return m_colored_illuminance;
	}
	
	/// @}
	
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
	void set_shadow_framebuffer(std::shared_ptr<gl::framebuffer> framebuffer) noexcept;
	
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
	[[nodiscard]] inline bool is_shadow_caster() const noexcept
	{
		return m_shadow_caster;
	}
	
	/// Returns the shadow map framebuffer, of `nullptr` if no shadow map framebuffer is set.
	[[nodiscard]] inline const std::shared_ptr<gl::framebuffer>& get_shadow_framebuffer() const noexcept
	{
		return m_shadow_framebuffer;
	}
	
	/// Returns the shadow bias factor.
	[[nodiscard]] inline float get_shadow_bias() const noexcept
	{
		return m_shadow_bias;
	}
	
	/// Returns the number of shadow cascades.
	[[nodiscard]] inline unsigned int get_shadow_cascade_count() const noexcept
	{
		return m_shadow_cascade_count;
	}
	
	/// Returns the shadow cascade coverage factor.
	[[nodiscard]] inline float get_shadow_cascade_coverage() const noexcept
	{
		return m_shadow_cascade_coverage;
	}
	
	/// Returns the shadow cascade distribution weight.
	[[nodiscard]] inline float get_shadow_cascade_distribution() const noexcept
	{
		return m_shadow_cascade_distribution;
	}
	
	/// Returns the array of shadow cascade far clipping plane distances.
	/// @{
	[[nodiscard]] inline constexpr std::span<const float> get_shadow_cascade_distances() const noexcept
	{
		return m_shadow_cascade_distances;
	}
	[[nodiscard]] inline constexpr std::span<float> get_shadow_cascade_distances() noexcept
	{
		return m_shadow_cascade_distances;
	}
	/// @}
	
	/// Returns the array of shadow cascade bias-scale matrices.
	[[nodiscard]] inline constexpr std::span<const math::fmat4> get_shadow_bias_scale_matrices() const noexcept
	{
		return m_shadow_bias_scale_matrices;
	}
	
	/// Returns the array of world-space to cascade texture-space transformation matrices.
	/// @{
	[[nodiscard]] inline constexpr std::span<const math::fmat4> get_shadow_cascade_matrices() const noexcept
	{
		return m_shadow_cascade_matrices;
	}
	[[nodiscard]] inline constexpr std::span<math::fmat4> get_shadow_cascade_matrices() noexcept
	{
		return m_shadow_cascade_matrices;
	}
	/// @}
	
	/// @}

private:
	void transformed() override;
	void color_updated();
	void illuminance_updated();
	void update_shadow_bias_scale_matrices();
	
	math::fvec3 m_direction{0.0f, 0.0f, -1.0f};
	math::fvec3 m_color{1.0f, 1.0f, 1.0f};
	float m_illuminance{};
	math::fvec3 m_colored_illuminance{};
	
	bool m_shadow_caster{false};
	std::shared_ptr<gl::framebuffer> m_shadow_framebuffer{nullptr};
	float m_shadow_bias{0.001f};
	unsigned int m_shadow_cascade_count{4};
	float m_shadow_cascade_coverage{1.0f};
	float m_shadow_cascade_distribution{0.8f};
	std::vector<float> m_shadow_cascade_distances;
	std::vector<math::fmat4> m_shadow_cascade_matrices;
	std::vector<math::fmat4> m_shadow_bias_scale_matrices;
};

} // namespace scene

#endif // ANTKEEPER_SCENE_DIRECTIONAL_LIGHT_HPP

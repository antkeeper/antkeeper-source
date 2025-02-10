// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_SCENE_DIRECTIONAL_LIGHT_HPP
#define ANTKEEPER_SCENE_DIRECTIONAL_LIGHT_HPP

#include <engine/scene/light.hpp>
#include <engine/gl/framebuffer.hpp>
#include <engine/gl/texture.hpp>
#include <engine/math/vector.hpp>
#include <memory>
#include <span>

namespace scene {

/// Light source with parallel rays and constant intensity.
class directional_light: public light
{
public:
	/// Creates a directional light.
	directional_light();
	
	/// Destructs a directional light.
	~directional_light() override = default;
	
	/// Returns light_type::directional.
	[[nodiscard]] inline light_type get_light_type() const noexcept override
	{
		return light_type::directional;
	}
	
	/// @name Light
	/// @{
	
	/// Sets the direction of the directional light.
	/// @param direction Unit-length light direction vector.
	void set_direction(const math::fvec3& direction);
	
	/// Sets the color of the light.
	/// @param color Light color.
	inline void set_color(const math::fvec3& color) noexcept
	{
		m_color = color;
		color_updated();
	}
	
	/// Sets the illuminance of the light on a surface perpendicular to the light direction.
	/// @param illuminance Illuminance on a surface perpendicular to the light direction.
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
	
	/// Enables or disables shadow casting.
	/// @param caster `true` if the light should cast shadows, `false` otherwise.
	void set_shadow_caster(bool caster) noexcept;
	
	/// Sets the shadow map framebuffer.
	/// @param framebuffer Pointer to a shadow map framebuffer.
	void set_shadow_framebuffer(std::shared_ptr<gl::framebuffer> framebuffer) noexcept;
	
	/// Sets the shadow bias factor for reducing self-shadowing.
	/// @param bias Shadow bias factor.
	void set_shadow_bias(float bias) noexcept;
	
	/// Sets the number of shadow cascades.
	/// @param count Number of shadow cascades, on `[1, 4]`.
	/// @note The number of shadow cascades will be clamped to `[1, 4]`.
	void set_shadow_cascade_count(unsigned int count) noexcept;
	
	/// Sets the maximum distance from a camera's near clipping plane up to which shadows are visible.
	/// @param distance Maximum shadow distance.
	void set_shadow_max_distance(float distance) noexcept;
	
	/// Sets the distance from the maximum shadow distance at which shadows will begin to fade out.
	/// @param range Shadow fade range.
	void set_shadow_fade_range(float range) noexcept;
	
	/// Sets the shadow cascade distribution.
	/// @param weight Linear interpolation weight between uniform and logarithmic cascade distributions. A weight of `0.0` results in a uniform cascade distribution, while `1.0` results in a logarithmic distribution.
	void set_shadow_cascade_distribution(float weight) noexcept;
	
	/// Returns `true` if the light casts shadows, `false` otherwise.
	[[nodiscard]] inline constexpr bool is_shadow_caster() const noexcept
	{
		return m_shadow_caster;
	}
	
	/// Returns the shadow map framebuffer, of `nullptr` if no shadow map framebuffer is set.
	[[nodiscard]] inline constexpr const std::shared_ptr<gl::framebuffer>& get_shadow_framebuffer() const noexcept
	{
		return m_shadow_framebuffer;
	}
	
	/// Returns the shadow map texture.
	[[nodiscard]] inline constexpr const std::shared_ptr<gl::texture_2d>& get_shadow_texture() const noexcept
	{
		return m_shadow_texture;
	}
	
	/// Returns the shadow bias factor.
	[[nodiscard]] inline constexpr float get_shadow_bias() const noexcept
	{
		return m_shadow_bias;
	}
	
	/// Returns the number of shadow cascades.
	[[nodiscard]] inline constexpr unsigned int get_shadow_cascade_count() const noexcept
	{
		return m_shadow_cascade_count;
	}
	
	/// Returns the maximum distance from a camera's near clipping plane up to which shadows are visible.
	[[nodiscard]] inline constexpr float get_shadow_max_distance() const noexcept
	{
		return m_shadow_max_distance;
	}
	
	/// Returns the distance from the maximum shadow distance at which shadows will begin to fade out.
	[[nodiscard]] inline constexpr float get_shadow_fade_range() const noexcept
	{
		return m_shadow_fade_range;
	}
	
	/// Returns the shadow cascade distribution weight.
	[[nodiscard]] inline constexpr float get_shadow_cascade_distribution() const noexcept
	{
		return m_shadow_cascade_distribution;
	}
	
	/// Returns the array of shadow cascade far clipping plane distances.
	[[nodiscard]] inline constexpr math::fvec4& get_shadow_cascade_distances() noexcept
	{
		return m_shadow_cascade_distances;
	}

	/// @copydoc get_shadow_cascade_distances()
	[[nodiscard]] inline constexpr const math::fvec4& get_shadow_cascade_distances() const noexcept
	{
		return m_shadow_cascade_distances;
	}
	
	/// Returns the array of shadow cascade scale-bias matrices.
	[[nodiscard]] inline constexpr std::span<const math::fmat4> get_shadow_scale_bias_matrices() const noexcept
	{
		return m_shadow_scale_bias_matrices;
	}
	
	/// Returns the array of world-space to cascade texture-space transformation matrices.
	[[nodiscard]] inline constexpr std::span<math::fmat4> get_shadow_cascade_matrices() noexcept
	{
		return m_shadow_cascade_matrices;
	}

	/// @copydoc get_shadow_cascade_matrices()
	[[nodiscard]] inline constexpr std::span<const math::fmat4> get_shadow_cascade_matrices() const noexcept
	{
		return m_shadow_cascade_matrices;
	}
	
	/// @}

private:
	void transformed() override;
	void color_updated();
	void illuminance_updated();
	void update_shadow_scale_bias_matrices();
	void update_shadow_cascade_distances();
	
	math::fvec3 m_direction{0.0f, 0.0f, -1.0f};
	math::fvec3 m_color{1.0f, 1.0f, 1.0f};
	float m_illuminance{};
	math::fvec3 m_colored_illuminance{};
	
	bool m_shadow_caster{false};
	std::shared_ptr<gl::framebuffer> m_shadow_framebuffer;
	std::shared_ptr<gl::texture_2d> m_shadow_texture;
	float m_shadow_bias{0.005f};
	unsigned int m_shadow_cascade_count{4};
	float m_shadow_max_distance{100.0f};
	float m_shadow_fade_range{0.0f};
	float m_shadow_cascade_distribution{0.8f};
	math::fvec4 m_shadow_cascade_distances{};
	math::fmat4 m_shadow_cascade_matrices[4]{};
	math::fmat4 m_shadow_scale_bias_matrices[4]{};
};

} // namespace scene

#endif // ANTKEEPER_SCENE_DIRECTIONAL_LIGHT_HPP

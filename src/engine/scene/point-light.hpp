// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_SCENE_POINT_LIGHT_HPP
#define ANTKEEPER_SCENE_POINT_LIGHT_HPP

#include <engine/scene/light.hpp>
#include <engine/math/vector.hpp>

namespace scene {

/**
 * Light source that radiates outward from a point.
 */
class point_light: public light
{
public:
	/// Returns light_type::point.
	[[nodiscard]] inline light_type get_light_type() const noexcept override
	{
		return light_type::point;
	}
	
	/**
	 * Sets the luminous flux of the light.
	 *
	 * @param luminous_flux Luminous flux.
	 */
	inline void set_luminous_flux(float luminous_flux) noexcept
	{
		m_luminous_flux = luminous_flux;
		luminous_flux_updated();
	}
	
	/// Returns the luminous flux of the light.
	[[nodiscard]] inline constexpr float get_luminous_flux() const noexcept
	{
		return m_luminous_flux;
	}
	
	/// Returns the color-modulated luminous flux of the light.
	[[nodiscard]] inline constexpr const math::fvec3& get_colored_luminous_flux() const noexcept
	{
		return m_colored_luminous_flux;
	}

private:
	void color_updated() override;
	void luminous_flux_updated() noexcept;
	
	float m_luminous_flux{};
	math::fvec3 m_colored_luminous_flux{};
};

} // namespace scene

#endif // ANTKEEPER_SCENE_POINT_LIGHT_HPP

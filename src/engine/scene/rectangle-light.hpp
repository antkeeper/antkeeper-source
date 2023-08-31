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

#ifndef ANTKEEPER_SCENE_RECTANGLE_LIGHT_HPP
#define ANTKEEPER_SCENE_RECTANGLE_LIGHT_HPP

#include <engine/scene/light.hpp>
#include <engine/math/vector.hpp>

namespace scene {

/**
 * Rectangular area light.
 */
class rectangle_light: public light
{
public:
	/// Constructs a rectangular area light.
	rectangle_light();
	
	/// Returns light_type::rectangle.
	[[nodiscard]] inline light_type get_light_type() const noexcept override
	{
		return light_type::rectangle;
	}
	
	/**
	 * Sets the size of the light.
	 *
	 * @param size Dimensions of the light.
	 */
	void set_size(const math::fvec2& size);
	
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
	
	/**
	 * Sets the luminance of the light.
	 *
	 * @param luminance Luminance.
	 */
	inline void set_luminance(float luminance) noexcept
	{
		m_luminance = luminance;
		luminance_updated();
	}
	
	/// Returns the dimensions of the light.
	[[nodiscard]] inline constexpr math::fvec2 get_size() const noexcept
	{
		return math::fvec2(get_scale());
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
	
	/// Returns the luminance of the light.
	[[nodiscard]] inline constexpr float get_luminance() const noexcept
	{
		return m_luminance;
	}
	
	/// Returns the color-modulated luminance of the light.
	[[nodiscard]] inline constexpr const math::fvec3& get_colored_luminance() const noexcept
	{
		return m_colored_luminance;
	}
	
	/// Returns the world-space positions of the light corners.
	[[nodiscard]] inline constexpr std::span<const math::fvec3, 4> get_corners() const noexcept
	{
		return m_corners;
	}

private:
	void transformed() override;
	void color_updated() override;
	void area_updated() noexcept;
	void luminous_flux_updated() noexcept;
	void luminance_updated() noexcept;
	
	float m_area{1.0f};
	math::fvec3 m_corners[4];
	float m_luminous_flux{};
	math::fvec3 m_colored_luminous_flux{};
	float m_luminance{};
	math::fvec3 m_colored_luminance{};
};

} // namespace scene

#endif // ANTKEEPER_SCENE_RECTANGLE_LIGHT_HPP

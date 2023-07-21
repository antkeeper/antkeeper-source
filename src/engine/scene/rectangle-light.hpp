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
	rectangle_light();
	
	/// Returns light_type::rectangle.
	[[nodiscard]] inline light_type get_light_type() const noexcept override
	{
		return light_type::rectangle;
	}
	
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
	
	/// Returns the color of the light.
	[[nodiscard]] inline const math::fvec3& get_color() const noexcept
	{
		return m_color;
	}
	
	/// Returns the luminous flux of the light.
	[[nodiscard]] inline float get_luminous_flux() const noexcept
	{
		return m_luminous_flux;
	}
	
	/// Returns the color-modulated luminous flux of the light.
	[[nodiscard]] inline const math::fvec3& get_colored_luminous_flux() const noexcept
	{
		return m_colored_luminous_flux;
	}
	
	/// Returns the luminance of the light.
	[[nodiscard]] inline float get_luminance() const noexcept
	{
		return m_luminance;
	}
	
	/// Returns the color-modulated luminance of the light.
	[[nodiscard]] inline const math::fvec3& get_colored_luminance() const noexcept
	{
		return m_colored_luminance;
	}
	
	/// Returns the positions of the light corners.
	[[nodiscard]] inline std::span<const math::fvec3, 4> get_corners() const noexcept
	{
		return m_corners;
	}

private:
	void transformed() override;
	void area_updated();
	void color_updated();
	void luminous_flux_updated();
	void luminance_updated();
	
	float m_area{1.0f};
	math::fvec3 m_corners[4];
	math::fvec3 m_color{1.0f, 1.0f, 1.0f};
	float m_luminous_flux{};
	math::fvec3 m_colored_luminous_flux{};
	float m_luminance{};
	math::fvec3 m_colored_luminance;
};

} // namespace scene

#endif // ANTKEEPER_SCENE_RECTANGLE_LIGHT_HPP

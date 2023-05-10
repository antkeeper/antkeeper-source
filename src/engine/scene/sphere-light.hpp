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

#ifndef ANTKEEPER_SCENE_SPHERE_LIGHT_HPP
#define ANTKEEPER_SCENE_SPHERE_LIGHT_HPP

#include <engine/scene/light.hpp>
#include <engine/math/vector.hpp>

namespace scene {

/**
 * Light source that radiates outward from a sphere.
 */
class sphere_light: public light
{
public:
	/// Returns light_type::sphere.
	[[nodiscard]] inline light_type get_light_type() const noexcept override
	{
		return light_type::sphere;
	}
	
	/**
	 * Sets the color of the sphere light.
	 *
	 * @param color Light color.
	 */
	inline void set_color(const math::vector<float, 3>& color) noexcept
	{
		m_color = color;
		update_spectral_luminous_power();
	}
	
	/**
	 * Sets the luminous power of the sphere light.
	 *
	 * @param luminous_power Luminous power.
	 */
	inline void set_luminous_power(float luminous_power) noexcept
	{
		m_luminous_power = luminous_power;
		update_spectral_luminous_power();
	}
	
	/**
	 * Sets the radius of the sphere light.
	 *
	 * @param radius Radius of the sphere light.
	 */
	inline void set_radius(float radius) noexcept
	{
		m_radius = radius;
	}
	
	/// Returns the color of the sphere light.
	[[nodiscard]] inline const math::vector<float, 3>& get_color() const noexcept
	{
		return m_color;
	}
	
	/// Returns the luminous power of the sphere light.
	[[nodiscard]] inline float get_luminous_power() const noexcept
	{
		return m_luminous_power;
	}
	
	/// Returns the spectral luminous power of the sphere light.
	[[nodiscard]] inline const math::vector<float, 3>& get_spectral_luminous_power() const noexcept
	{
		return m_spectral_luminous_power;
	}
	
	/// Returns the radius of the sphere light.
	[[nodiscard]] inline float get_radius() const noexcept
	{
		return m_radius;
	}
	
	/// Calculates and returns the luminance of the sphere light.
	[[nodiscard]] float get_luminance() const noexcept;
	
	/// Calculates and returns the spectral luminance of the sphere light.
	[[nodiscard]] math::vector<float, 3> get_spectral_luminance() const noexcept;

private:
	inline void update_spectral_luminous_power() noexcept
	{
		m_spectral_luminous_power = m_color * m_luminous_power;
	}
	
	math::vector<float, 3> m_color{1.0f, 1.0f, 1.0f};
	float m_luminous_power{};
	math::vector<float, 3> m_spectral_luminous_power{};
	float m_radius{};
};

} // namespace scene

#endif // ANTKEEPER_SCENE_SPHERE_LIGHT_HPP

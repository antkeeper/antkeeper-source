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

#ifndef ANTKEEPER_SCENE_LIGHT_HPP
#define ANTKEEPER_SCENE_LIGHT_HPP

#include <engine/scene/object.hpp>
#include <engine/scene/light-type.hpp>

namespace scene {

/**
 * Abstract base class for light objects.
 */
class light: public object<light>
{
public:
	/// Returns an enumeration denoting the light object type.
	[[nodiscard]] virtual light_type get_light_type() const noexcept = 0;
	
	[[nodiscard]] inline const aabb_type& get_bounds() const noexcept override
	{
		return m_bounds;
	}
	
	/**
	 * Sets the color of the light.
	 *
	 * @param color Scene-linear RGB color, on `[0, 1]`.
	 */
	void set_color(const math::fvec3& color);
	
	/**
	 * Sets the color of the light from a color temperature.
	 *
	 * @param temperature Color temperature, in Kelvin.
	 */
	void set_color_temperature(float temperature);
	
	/// Returns the scene-linear RGB color of the light.
	[[nodiscard]] inline constexpr const math::fvec3& get_color() const noexcept
	{
		return m_color;
	}
	
protected:
	/// Called each time the light color is modified.
	inline virtual void color_updated() {};

private:
	void transformed() override;
	
	aabb_type m_bounds{};
	
	math::fvec3 m_color{1.0f, 1.0f, 1.0f};
};

} // namespace scene

#endif // ANTKEEPER_SCENE_LIGHT_HPP

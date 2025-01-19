// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_SCENE_LIGHT_HPP
#define ANTKEEPER_SCENE_LIGHT_HPP

#include <engine/scene/object.hpp>
#include <engine/scene/light-type.hpp>

namespace scene {

/// Abstract base class for light objects.
class light: public object<light>
{
public:
	/// Destructs a light.
	~light() override = default;
	
	/// Returns an enumeration denoting the light object type.
	[[nodiscard]] virtual light_type get_light_type() const noexcept = 0;
	
	[[nodiscard]] inline const aabb_type& get_bounds() const noexcept override
	{
		return m_bounds;
	}
	
	/// Sets the color of the light.
	/// @param color Scene-linear RGB color, on `[0, 1]`.
	void set_color(const math::fvec3& color);
	
	/// Sets the color of the light from a color temperature.
	/// @param temperature Color temperature, in Kelvin.
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

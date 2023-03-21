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

#ifndef ANTKEEPER_PHYSICS_SPHERE_COLLIDER_HPP
#define ANTKEEPER_PHYSICS_SPHERE_COLLIDER_HPP

#include <engine/physics/kinematics/collider.hpp>
#include <engine/geom/primitives/sphere.hpp>

namespace physics {

/**
 * Sphere collision object.
 */
class sphere_collider: public collider
{
public:
	/// Sphere type.
	using sphere_type = geom::sphere<float>;
	
	[[nodiscard]] inline constexpr collider_type type() const noexcept override
	{
		return collider_type::sphere;
	}
	
	/**
	 * Constructs a sphere collider from a sphere.
	 *
	 * @param sphere Sphere shape.
	 */
	inline explicit sphere_collider(const sphere_type& sphere) noexcept:
		m_sphere{sphere}
	{}
	
	/**
	 * Constructs a sphere collider.
	 *
	 * @param center Sphere center.
	 * @param radius Sphere radius.
	 */
	/// @{
	inline sphere_collider(const math::vector<float, 3>& center, float radius) noexcept:
		m_sphere{center, radius}
	{}
	inline explicit sphere_collider(float radius) noexcept:
		m_sphere{{0.0f, 0.0f, 0.0f}, radius}
	{}
	sphere_collider() noexcept = default;
	/// @}
	
	/**
	 * Sets the collider's sphere.
	 *
	 * @param sphere Sphere shape.
	 */
	inline void set_sphere(const sphere_type& sphere) noexcept
	{
		m_sphere = sphere;
	}
	
	/**
	 * Sets the center of the sphere.
	 *
	 * @param center Sphere center, in object space.
	 */
	inline void set_center(const math::vector<float, 3>& center) noexcept
	{
		m_sphere.center = center;
	}
	
	/**
	 * Sets the radius of the sphere.
	 *
	 * @param radius Sphere radius.
	 */
	inline void set_radius(float radius) noexcept
	{
		m_sphere.radius = radius;
	}
	
	/// Returns the sphere shape.
	[[nodiscard]] inline const sphere_type& get_sphere() const noexcept
	{
		return m_sphere;
	}
	
	/// Returns the center of the sphere, in object space.
	[[nodiscard]] inline const math::vector<float, 3>& get_center() const noexcept
	{
		return m_sphere.center;
	}
	
	/// Returns the radius of the sphere.
	[[nodiscard]] inline float get_radius() const noexcept
	{
		return m_sphere.radius;
	}
	
private:
	sphere_type m_sphere{{0.0f, 0.0f, 0.0f}, 0.0f};
};

} // namespace physics

#endif // ANTKEEPER_PHYSICS_SPHERE_COLLIDER_HPP

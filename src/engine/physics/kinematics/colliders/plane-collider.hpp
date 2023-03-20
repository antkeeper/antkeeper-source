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

#ifndef ANTKEEPER_PHYSICS_PLANE_COLLIDER_HPP
#define ANTKEEPER_PHYSICS_PLANE_COLLIDER_HPP

#include <engine/physics/kinematics/collider.hpp>
#include <engine/geom/primitives/plane.hpp>

namespace physics {

/**
 * Plane collision object.
 */
class plane_collider: public collider
{
public:
	/// Plane type.
	using plane_type = geom::primitive::plane<float>;
	
	[[nodiscard]] inline constexpr collider_type type() const noexcept override
	{
		return collider_type::plane;
	}
	
	/**
	 * Constructs a plane collider from a plane.
	 *
	 * @param plane Plane shape.
	 */
	inline explicit plane_collider(const plane_type& plane) noexcept:
		m_plane{plane}
	{}
	
	/**
	 * Constructs a plane collider from a normal and constant.
	 *
	 * @param normal Plane normal, in object space.
	 * @param constant Plane constant.
	 */
	/// @{
	inline plane_collider(const math::vector<float, 3>& normal, float constant) noexcept:
		m_plane{normal, constant}
	{}
	inline explicit plane_collider(const math::vector<float, 3>& normal) noexcept:
		m_plane{normal, 0.0f}
	{}
	plane_collider() noexcept = default;
	/// @}
	
	/**
	 * Constructs a plane collider from a normal and offset.
	 *
	 * @param normal Plane normal, in object space.
	 * @param offset Offset from the origin, in object space.
	 */
	inline plane_collider(const math::vector<float, 3>& normal, const math::vector<float, 3>& offset) noexcept:
		m_plane(normal, offset)
	{}
	
	/**
	 * Sets the collider's plane.
	 *
	 * @param plane Plane shape.
	 */
	inline void set_plane(const plane_type& plane) noexcept
	{
		m_plane = plane;
	}
	
	/**
	 * Sets the plane normal.
	 *
	 * @param normal Plane normal, in object space.
	 */
	inline void set_normal(const math::vector<float, 3>& normal) noexcept
	{
		m_plane.normal = normal;
	}
	
	/**
	 * Sets the plane constant.
	 *
	 * @param constant Plane constant.
	 */
	inline void set_constant(float constant) noexcept
	{
		m_plane.constant = constant;
	}
	
	/// Returns the plane shape.
	[[nodiscard]] inline const plane_type& get_plane() const noexcept
	{
		return m_plane;
	}
	
	/// Returns the plane normal, in object space.
	[[nodiscard]] inline const math::vector<float, 3>& get_normal() const noexcept
	{
		return m_plane.normal;
	}
	
	/// Returns the plane constant.
	[[nodiscard]] inline float get_constant() const noexcept
	{
		return m_plane.constant;
	}
	
private:
	plane_type m_plane{{0.0f, 0.0f, 0.0f}, 0.0f};
};

} // namespace physics

#endif // ANTKEEPER_PHYSICS_PLANE_COLLIDER_HPP

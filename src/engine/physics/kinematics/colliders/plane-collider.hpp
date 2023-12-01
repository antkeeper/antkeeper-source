// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

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
	using plane_type = geom::plane<float>;
	
	[[nodiscard]] inline constexpr collider_type type() const noexcept override
	{
		return collider_type::plane;
	}
	
	/**
	 * Constructs a plane collider from a plane.
	 *
	 * @param plane Plane shape.
	 */
	inline constexpr explicit plane_collider(const plane_type& plane) noexcept:
		m_plane{plane}
	{}
	
	/**
	 * Constructs a plane collider from a normal and constant.
	 *
	 * @param normal Plane normal, in object space.
	 * @param constant Plane constant.
	 */
	/// @{
	inline constexpr plane_collider(const math::fvec3& normal, float constant) noexcept:
		m_plane{normal, constant}
	{}
	inline constexpr explicit plane_collider(const math::fvec3& normal) noexcept:
		m_plane{normal, 0.0f}
	{}
	constexpr plane_collider() noexcept = default;
	/// @}
	
	/**
	 * Constructs a plane collider from a normal and offset.
	 *
	 * @param normal Plane normal, in object space.
	 * @param offset Offset from the origin, in object space.
	 */
	inline constexpr plane_collider(const math::fvec3& normal, const math::fvec3& offset) noexcept:
		m_plane(normal, offset)
	{}
	
	/**
	 * Sets the collider's plane.
	 *
	 * @param plane Plane shape.
	 */
	inline constexpr void set_plane(const plane_type& plane) noexcept
	{
		m_plane = plane;
	}
	
	/**
	 * Sets the plane normal.
	 *
	 * @param normal Plane normal, in object space.
	 */
	inline constexpr void set_normal(const math::fvec3& normal) noexcept
	{
		m_plane.normal = normal;
	}
	
	/**
	 * Sets the plane constant.
	 *
	 * @param constant Plane constant.
	 */
	inline constexpr void set_constant(float constant) noexcept
	{
		m_plane.constant = constant;
	}
	
	/// Returns the plane shape.
	[[nodiscard]] inline constexpr const plane_type& get_plane() const noexcept
	{
		return m_plane;
	}
	
	/// Returns the plane normal, in object space.
	[[nodiscard]] inline constexpr const math::fvec3& get_normal() const noexcept
	{
		return m_plane.normal;
	}
	
	/// Returns the plane constant.
	[[nodiscard]] inline constexpr float get_constant() const noexcept
	{
		return m_plane.constant;
	}
	
private:
	plane_type m_plane{};
};

} // namespace physics

#endif // ANTKEEPER_PHYSICS_PLANE_COLLIDER_HPP

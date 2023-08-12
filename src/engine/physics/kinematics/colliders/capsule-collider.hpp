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

#ifndef ANTKEEPER_PHYSICS_CAPSULE_COLLIDER_HPP
#define ANTKEEPER_PHYSICS_CAPSULE_COLLIDER_HPP

#include <engine/physics/kinematics/collider.hpp>
#include <engine/geom/primitives/capsule.hpp>

namespace physics {

/**
 * Capsule collision object.
 */
class capsule_collider: public collider
{
public:
	/// Capule type.
	using capsule_type = geom::capsule<float>;
	
	[[nodiscard]] inline constexpr collider_type type() const noexcept override
	{
		return collider_type::capsule;
	}
	
	/**
	 * Constructs a capsule collider from a capsule.
	 *
	 * @param capsule Capsule shape.
	 */
	inline constexpr explicit capsule_collider(const capsule_type& capsule) noexcept:
		m_capsule{capsule}
	{}
	
	/**
	 * Constructs a capsule collider.
	 *
	 * @param segment Capsule line segment.
	 * @param radius Capsule hemisphere radius.
	 */
	/// @{
	inline constexpr capsule_collider(const capsule_type::segment_type& segment, float radius) noexcept:
		m_capsule{segment, radius}
	{}
	constexpr capsule_collider() noexcept = default;
	/// @}
	
	/**
	 * Sets the collider's capsule.
	 *
	 * @param capsule Capsule shape.
	 */
	inline constexpr void set_capsule(const capsule_type& capsule) noexcept
	{
		m_capsule = capsule;
	}
	
	/**
	 * Sets the segment of the capsule.
	 *
	 * @param segment Capsule segment, in object space.
	 */
	inline constexpr void set_segment(const capsule_type::segment_type& segment) noexcept
	{
		m_capsule.segment = segment;
	}
	
	/**
	 * Sets the radius of the capsule hemispheres.
	 *
	 * @param radius Capsule hemisphere radius.
	 */
	inline constexpr void set_radius(float radius) noexcept
	{
		m_capsule.radius = radius;
	}
	
	/// Returns the capsule shape.
	[[nodiscard]] inline constexpr const capsule_type& get_capsule() const noexcept
	{
		return m_capsule;
	}
	
	/// Returns the segment of the capsule, in object space.
	[[nodiscard]] inline constexpr const capsule_type::segment_type& get_segment() const noexcept
	{
		return m_capsule.segment;
	}
	
	/// Returns the radius of the capsule hemispheres.
	[[nodiscard]] inline constexpr float get_radius() const noexcept
	{
		return m_capsule.radius;
	}
	
private:
	capsule_type m_capsule{};
};

} // namespace physics

#endif // ANTKEEPER_PHYSICS_CAPSULE_COLLIDER_HPP

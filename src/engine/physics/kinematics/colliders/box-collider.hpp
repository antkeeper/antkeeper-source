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

#ifndef ANTKEEPER_PHYSICS_BOX_COLLIDER_HPP
#define ANTKEEPER_PHYSICS_BOX_COLLIDER_HPP

#include <engine/physics/kinematics/collider.hpp>
#include <engine/geom/primitives/box.hpp>

namespace physics {

/**
 * Box collision object.
 */
class box_collider: public collider
{
public:
	/// Box type.
	using box_type = geom::box<float>;
	
	[[nodiscard]] inline constexpr collider_type type() const noexcept override
	{
		return collider_type::box;
	}
	
	/**
	 * Constructs a box collider from a box.
	 *
	 * @param box Box shape.
	 */
	inline constexpr explicit box_collider(const box_type& box) noexcept:
		m_box{box}
	{}
	
	/**
	 * Constructs a box collider.
	 *
	 * @param min Minimum extent of the box, in object space.
	 * @param max Maximum extent of the box, in object space.
	 */
	/// @{
	inline constexpr box_collider(const math::fvec3& min, const math::fvec3& max) noexcept:
		m_box{min, max}
	{}
	constexpr box_collider() noexcept = default;
	/// @}
	
	/**
	 * Sets the collider's box.
	 *
	 * @param box Box shape.
	 */
	inline constexpr void set_box(const box_type& box) noexcept
	{
		m_box = box;
	}
	
	/**
	 * Sets the minimum extent of the box.
	 *
	 * @param min Minimum extent of the box, in object space.
	 */
	inline constexpr void set_min(const math::fvec3& min) noexcept
	{
		m_box.min = min;
	}
	
	/**
	 * Sets the maximum extent of the box.
	 *
	 * @param max Maximum extent of the box, in object space.
	 */
	inline constexpr void set_max(const math::fvec3& max) noexcept
	{
		m_box.max = max;
	}
	
	/// Returns the box shape.
	[[nodiscard]] inline constexpr const box_type& get_box() const noexcept
	{
		return m_box;
	}
	
	/// Returns the minimum extent of the box, in object space.
	[[nodiscard]] inline constexpr const math::fvec3& get_min() const noexcept
	{
		return m_box.min;
	}
	
	/// Returns the maximum extent of the box, in object space.
	[[nodiscard]] inline constexpr const math::fvec3& get_max() const noexcept
	{
		return m_box.max;
	}
	
private:
	box_type m_box{};
};

} // namespace physics

#endif // ANTKEEPER_PHYSICS_BOX_COLLIDER_HPP

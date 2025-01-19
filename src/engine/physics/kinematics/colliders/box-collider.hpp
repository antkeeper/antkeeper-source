// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_PHYSICS_BOX_COLLIDER_HPP
#define ANTKEEPER_PHYSICS_BOX_COLLIDER_HPP

#include <engine/physics/kinematics/collider.hpp>
#include <engine/geom/primitives/box.hpp>

namespace physics {

/// Box collision object.
class box_collider: public collider
{
public:
	/// Box type.
	using box_type = geom::box<float>;
	
	[[nodiscard]] inline constexpr collider_type type() const noexcept override
	{
		return collider_type::box;
	}
	
	/// Constructs a box collider from a box.
	/// @param box Box shape.
	inline constexpr explicit box_collider(const box_type& box) noexcept:
		m_box{box}
	{}
	
	/// Constructs a box collider.
	/// @param min Minimum extent of the box, in object space.
	/// @param max Maximum extent of the box, in object space.
	inline constexpr box_collider(const math::fvec3& min, const math::fvec3& max) noexcept:
		m_box{min, max}
	{}

	/// Constructs a box collider.
	constexpr box_collider() noexcept = default;
	
	/// Destructs a box collider.
	~box_collider() override = default;
	
	/// Sets the collider's box.
	/// @param box Box shape.
	inline constexpr void set_box(const box_type& box) noexcept
	{
		m_box = box;
	}
	
	/// Sets the minimum extent of the box.
	/// @param min Minimum extent of the box, in object space.
	inline constexpr void set_min(const math::fvec3& min) noexcept
	{
		m_box.min = min;
	}
	
	/// Sets the maximum extent of the box.
	/// @param max Maximum extent of the box, in object space.
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

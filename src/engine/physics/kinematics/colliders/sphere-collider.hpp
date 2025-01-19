// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_PHYSICS_SPHERE_COLLIDER_HPP
#define ANTKEEPER_PHYSICS_SPHERE_COLLIDER_HPP

#include <engine/physics/kinematics/collider.hpp>
#include <engine/geom/primitives/sphere.hpp>

namespace physics {

/// Sphere collision object.
class sphere_collider: public collider
{
public:
	/// Sphere type.
	using sphere_type = geom::sphere<float>;
	
	[[nodiscard]] inline constexpr collider_type type() const noexcept override
	{
		return collider_type::sphere;
	}
	
	/// Constructs a sphere collider from a sphere.
	/// @param sphere Sphere shape.
	inline constexpr explicit sphere_collider(const sphere_type& sphere) noexcept:
		m_sphere{sphere}
	{}
	
	/// Constructs a sphere collider.
	/// @param center Sphere center.
	/// @param radius Sphere radius.
	inline constexpr sphere_collider(const math::fvec3& center, float radius) noexcept:
		m_sphere{center, radius}
	{}

	/// Constructs a sphere collider.
	/// @param radius Sphere radius.
	inline constexpr explicit sphere_collider(float radius) noexcept:
		m_sphere{{}, radius}
	{}

	/// Constructs a sphere collider.
	constexpr sphere_collider() noexcept = default;
	
	/// Destructs a sphere collider.
	~sphere_collider() override = default;
	
	/// Sets the collider's sphere.
	/// @param sphere Sphere shape.
	inline constexpr void set_sphere(const sphere_type& sphere) noexcept
	{
		m_sphere = sphere;
	}
	
	/// Sets the center of the sphere.
	/// @param center Sphere center, in object space.
	inline constexpr void set_center(const math::fvec3& center) noexcept
	{
		m_sphere.center = center;
	}
	
	/// Sets the radius of the sphere.
	/// @param radius Sphere radius.
	inline constexpr void set_radius(float radius) noexcept
	{
		m_sphere.radius = radius;
	}
	
	/// Returns the sphere shape.
	[[nodiscard]] inline constexpr const sphere_type& get_sphere() const noexcept
	{
		return m_sphere;
	}
	
	/// Returns the center of the sphere, in object space.
	[[nodiscard]] inline constexpr const math::fvec3& get_center() const noexcept
	{
		return m_sphere.center;
	}
	
	/// Returns the radius of the sphere.
	[[nodiscard]] inline constexpr float get_radius() const noexcept
	{
		return m_sphere.radius;
	}
	
private:
	sphere_type m_sphere{};
};

} // namespace physics

#endif // ANTKEEPER_PHYSICS_SPHERE_COLLIDER_HPP

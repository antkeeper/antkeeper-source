// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GEOM_PRIMITIVES_HYPERSPHERE_HPP
#define ANTKEEPER_GEOM_PRIMITIVES_HYPERSPHERE_HPP

#include <engine/math/constants.hpp>
#include <engine/math/vector.hpp>

namespace geom {
namespace primitives {

/// Calculates the volume of a hypersphere.
/// @tparam T Real type.
/// @tparam N Number of dimensions.
/// @param r Hypersphere radius.
/// @return Hypersphere volume.
template <class T, std::size_t N>
[[nodiscard]] constexpr T hypersphere_volume([[maybe_unused]] T r) noexcept
{
	if constexpr (N == 0)
	{
		return T{1};
	}
	else if constexpr (N == 1)
	{
		return r * T{2};
	}
	else
	{
		return (math::two_pi<T> / static_cast<T>(N)) * r * r * hypersphere_volume<T, N - 2>(r);
	}
}

/// *n*-dimensional sphere.
/// @tparam T Real type.
/// @tparam N Number of dimensions.
template <class T, std::size_t N>
struct hypersphere
{
	/// Vector type.
	using vector_type = math::vector<T, N>;
	
	/// Hypersphere center.
	vector_type center;
	
	/// Hypersphere radius.
	T radius;
	
	/// Tests whether a point is contained within this hypersphere.
	/// @param point Point to test for containment.
	/// @return `true` if the point is contained within this hypersphere, `false` otherwise.
	[[nodiscard]] inline constexpr bool contains(const vector_type& point) const noexcept
	{
		return math::sqr_distance(center, point) <= radius * radius;
	}
	
	/// Tests whether another hypersphere is contained within this hypersphere.
	/// @param other Hypersphere to test for containment.
	/// @return `true` if the hypersphere is contained within this hypersphere, `false` otherwise.
	[[nodiscard]] constexpr bool contains(const hypersphere& other) const noexcept
	{
		const T containment_radius = radius - other.radius;
		if (containment_radius < T{0})
		{
			return false;
		}
		
		return math::sqr_distance(center, other.center) <= containment_radius * containment_radius;
	}
	
	/// Calculates the signed distance from the hypersphere to a point.
	/// @param point Input point.
	/// @return Signed distance from the hypersphere to @p point.
	[[nodiscard]] inline T distance(const vector_type& point) const noexcept
	{
		const T d = math::sqr_distance(center, point);
		return (d ? std::sqrt(d) : d) - radius;
	}
	
	/// Tests whether another hypersphere intersects this hypersphere.
	/// @param other Hypersphere to test for intersection.
	/// @return `true` if the hypersphere intersects this hypersphere, `false` otherwise.
	[[nodiscard]] constexpr bool intersects(const hypersphere& other) const noexcept
	{
		const T intersection_radius = radius + other.radius;
		return math::sqr_distance(center, other.center) <= intersection_radius * intersection_radius;
	}
	
	/// Calculates the volume of the hypersphere.
	[[nodiscard]] inline constexpr T volume() const noexcept
	{
		return hypersphere_volume<T, N>(radius);
	}
};

} // namespace primitives

using namespace primitives;

} // namespace geom

#endif // ANTKEEPER_GEOM_PRIMITIVES_HYPERSPHERE_HPP

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

#ifndef ANTKEEPER_GEOM_PRIMITIVES_HYPERSPHERE_HPP
#define ANTKEEPER_GEOM_PRIMITIVES_HYPERSPHERE_HPP

#include <engine/math/numbers.hpp>
#include <engine/math/vector.hpp>

namespace geom {
namespace primitives {

/**
 * *n*-dimensional sphere.
 *
 * @tparam T Real type.
 * @tparam N Number of dimensions.
 */
template <class T, std::size_t N>
struct hypersphere
{
	/// Vector type.
	using vector_type = math::vector<T, N>;
	
	/// Hypersphere center.
	vector_type center;
	
	/// Hypersphere radius.
	T radius;
	
	/**
	 * Tests whether a point is contained within this hypersphere.
	 *
	 * @param point Point to test for containment.
	 *
	 * @return `true` if the point is contained within this hypersphere, `false` otherwise.
	 */
	[[nodiscard]] inline constexpr bool contains(const vector_type& point) const noexcept
	{
		return math::sqr_distance(center, point) <= radius * radius;
	}
	
	/**
	 * Tests whether another hypersphere is contained within this hypersphere.
	 *
	 * @param other Hypersphere to test for containment.
	 *
	 * @return `true` if the hypersphere is contained within this hypersphere, `false` otherwise.
	 */
	[[nodiscard]] constexpr bool contains(const hypersphere& other) const noexcept
	{
		const T containment_radius = radius - other.radius;
		if (containment_radius < T{0})
		{
			return false;
		}
		
		return math::sqr_distance(center, other.center) <= containment_radius * containment_radius;
	}
	
	/**
	 * Calculates the signed distance from the hypersphere to a point.
	 *
	 * @param point Input point.
	 *
	 * @return Signed distance from the hypersphere to @p point.
	 */
	[[nodiscard]] inline T distance(const vector_type& point) const noexcept
	{
		const T d = math::sqr_distance(center, point);
		return (d ? std::sqrt(d) : d) - radius;
	}
	
	/**
	 * Tests whether another hypersphere intersects this hypersphere.
	 *
	 * @param other Hypersphere to test for intersection.
	 *
	 * @return `true` if the hypersphere intersects this hypersphere, `false` otherwise.
	 */
	[[nodiscard]] constexpr bool intersects(const hypersphere& other) const noexcept
	{
		const T intersection_radius = radius + other.radius;
		return math::sqr_distance(center, other.center) <= intersection_radius * intersection_radius;
	}
	
	/**
	 * Volume calculation helper function.
	 *
	 * @tparam M Dimension.
	 *
	 * @param r Radius.
	 *
	 * @return Volume.
	 */
	/// @private
	/// @{
	template <std::size_t M>
	[[nodiscard]] static constexpr T volume(T r) noexcept
	{
		return (math::two_pi<T> / static_cast<T>(M)) * r * r * volume<M - 2>(r);
	}
	
	template <>
	[[nodiscard]] static constexpr T volume<1>(T r) noexcept
	{
		return r * T{2};
	}
	
	template <>
	[[nodiscard]] static constexpr T volume<0>(T r) noexcept
	{
		return T{1};
	}
	/// @}
	
	/// Calculates the volume of the hypersphere.
	[[nodiscard]] inline constexpr T volume() const noexcept
	{
		return volume<N>(radius);
	}
};

} // namespace primitives

using namespace primitives;

} // namespace geom

#endif // ANTKEEPER_GEOM_PRIMITIVES_HYPERSPHERE_HPP

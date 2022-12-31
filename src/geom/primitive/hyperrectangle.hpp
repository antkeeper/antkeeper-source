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

#ifndef ANTKEEPER_GEOM_PRIMITIVE_HYPERRECTANGLE_HPP
#define ANTKEEPER_GEOM_PRIMITIVE_HYPERRECTANGLE_HPP

#include "math/vector.hpp"
#include <algorithm>

namespace geom {
namespace primitive {

/**
 * *n*-dimensional axis-aligned rectangle.
 *
 * @tparam T Real type.
 * @tparam N Number of dimensions.
 */
template <class T, std::size_t N>
struct hyperrectangle
{
	typedef math::vector<T, N> vector_type;
	
	/// Minimum extent of the hyperrectangle.
	vector_type min;
	
	/// Maximum extent of the hyperrectangle.
	vector_type max;
	
	/**
	 * Tests whether a point is contained within this hyperrectangle.
	 *
	 * @param point Point to test for containment.
	 *
	 * @return `true` if the point is contained within this hyperrectangle, `false` otherwise.
	 */
	constexpr bool contains(const vector_type& point) const noexcept
	{
		for (std::size_t i = 0; i < N; ++i)
			if (point[i] < min[i] || point[i] > max[i])
				return false;
		return true;
	}
	
	/**
	 * Tests whether another hyperrectangle is contained within this hyperrectangle.
	 *
	 * @param other Hyperrectangle to test for containment.
	 *
	 * @return `true` if the hyperrectangle is contained within this hyperrectangle, `false` otherwise.
	 */
	constexpr bool contains(const hyperrectangle& other) const noexcept
	{
		for (std::size_t i = 0; i < N; ++i)
			if (other.min[i] < min[i] || other.max[i] > max[i])
				return false;
		return true;
	}
	
	/// Returns the center position of the hyperrectangle.
	constexpr vector_type center() const noexcept
	{
		return (min + max) * T{0.5};
	}
	
	/**
	 * Calculates the signed distance from the hyperrectangle to a point.
	 *
	 * @param point Input point.
	 *
	 * @return Signed distance from the hyperrectangle to @p point.
	 */
	T distance(const vector_type& point) const noexcept
	{
		vector_type d = math::abs(point - center()) - size() * T{0.5};
		return math::length(math::max(vector_type::zero(), d)) + std::min<T>(T{0}, math::max(d));
	}
	
	/**
	 * Extends the hyperrectangle to include a point.
	 *
	 * @param point Point to include in the hyperrectangle.
	 */
	void extend(const vector_type& point) noexcept
	{
		min = math::min(min, point);
		max = math::max(max, point);
	}
	
	/**
	 * Extends the hyperrectangle to include another hyperrectangle.
	 *
	 * @param other Hyperrectangle to include in this hyperrectangle.
	 */
	void extend(const hyperrectangle& other) noexcept
	{
		min = math::min(min, other.min);
		max = math::max(max, other.max);
	}
	
	/**
	 * Tests whether another hyperrectangle intersects this hyperrectangle.
	 *
	 * @param other Hyperrectangle to test for intersection.
	 *
	 * @return `true` if the hyperrectangle intersects this hyperrectangle, `false` otherwise.
	 */
	constexpr bool intersects(const hyperrectangle& other) const noexcept
	{
		for (std::size_t i = 0; i < N; ++i)
			if (other.min[i] > max[i] || other.max[i] < min[i])
				return false;
		return true;
	}
	
	/// Returns the size of the hyperrectangle.
	constexpr vector_type size() const noexcept
	{
		return max - min;
	}
	
	/**
	 * Returns `false` if any coordinates of `min` are greater than `max`.
	 */
	constexpr bool valid() const noexcept
	{
		for (std::size_t i = 0; i < N; ++i)
			if (min[i] > max[i])
				return false;
		return true;
	}
	
	/// Calculates the volume of the hyperrectangle.
	constexpr T volume() const noexcept
	{
		T v = max[0] - min[0];
		for (std::size_t i = 1; i < N; ++i)
			v *= max[i] - min[i];
		return v;
	}
};

} // namespace primitive
} // namespace geom

#endif // ANTKEEPER_GEOM_PRIMITIVE_HYPERRECTANGLE_HPP

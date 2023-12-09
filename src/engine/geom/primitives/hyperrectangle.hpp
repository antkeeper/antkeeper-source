// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GEOM_PRIMITIVES_HYPERRECTANGLE_HPP
#define ANTKEEPER_GEOM_PRIMITIVES_HYPERRECTANGLE_HPP

#include <engine/math/vector.hpp>
#include <algorithm>

namespace geom {
namespace primitives {

/**
 * *n*-dimensional axis-aligned rectangle.
 *
 * @tparam T Real type.
 * @tparam N Number of dimensions.
 */
template <class T, std::size_t N>
struct hyperrectangle
{
	/// Vector type.
	using vector_type = math::vector<T, N>;
	
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
	[[nodiscard]] constexpr bool contains(const vector_type& point) const noexcept
	{
		for (std::size_t i = 0; i < N; ++i)
		{
			if (point[i] < min[i] || point[i] > max[i])
			{
				return false;
			}
		}
		return true;
	}
	
	/**
	 * Tests whether another hyperrectangle is contained within this hyperrectangle.
	 *
	 * @param other Hyperrectangle to test for containment.
	 *
	 * @return `true` if the hyperrectangle is contained within this hyperrectangle, `false` otherwise.
	 */
	[[nodiscard]] constexpr bool contains(const hyperrectangle& other) const noexcept
	{
		for (std::size_t i = 0; i < N; ++i)
		{
			if (other.min[i] < min[i] || other.max[i] > max[i])
			{
				return false;
			}
		}
		return true;
	}
	
	/// Returns the center position of the hyperrectangle.
	[[nodiscard]] inline constexpr vector_type center() const noexcept
	{
		return (min + max) / T{2};
	}
	
	/**
	 * Calculates the signed distance from the hyperrectangle to a point.
	 *
	 * @param point Input point.
	 *
	 * @return Signed distance from the hyperrectangle to @p point.
	 */
	[[nodiscard]] T distance(const vector_type& point) const noexcept
	{
		const vector_type d = math::abs(point - center()) - extents();
		return math::length(math::max(math::zero<vector_type>, d)) + std::min<T>(T{0}, math::max(d));
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
	[[nodiscard]] constexpr bool intersects(const hyperrectangle& other) const noexcept
	{
		for (std::size_t i = 0; i < N; ++i)
		{
			if (other.min[i] > max[i] || other.max[i] < min[i])
			{
				return false;
			}
		}
		return true;
	}
	
	/// Calculates the size of the hyperrectangle.
	[[nodiscard]] inline constexpr vector_type size() const noexcept
	{
		return max - min;
	}
	
	/// Calculates the extents of the hyperrectangle.
	[[nodiscard]] inline constexpr vector_type extents() const noexcept
	{
		return size() / T{2};
	}
	
	/**
	 * Returns `true` if any coordinates of `min` are greater than `max`, `false` otherwise.
	 */
	[[nodiscard]] constexpr bool degenerate() const noexcept
	{
		for (std::size_t i = 0; i < N; ++i)
		{
			if (min[i] > max[i])
			{
				return true;
			}
		}
		
		return false;
	}
	
	/// Calculates the volume of the hyperrectangle.
	[[nodiscard]] constexpr T volume() const noexcept
	{
		T v = max[0] - min[0];
		for (std::size_t i = 1; i < N; ++i)
		{
			v *= max[i] - min[i];
		}
		return v;
	}
	
	/**
	 * Returns the nth corner of the hyperrectangle.
	 *
	 * @param index Index of a corner.
	 */
	[[nodiscard]] constexpr vector_type corner(std::size_t index) const noexcept
	{
		vector_type p;
		
		for (std::size_t i = 0; i < N; ++i)
		{
			p[i] = ((index >> ((N - 1) - i)) & 1) ? max[i] : min[i];
		}
		
		return p;
	}
};

} // namespace primitives

using namespace primitives;

} // namespace geom

#endif // ANTKEEPER_GEOM_PRIMITIVES_HYPERRECTANGLE_HPP

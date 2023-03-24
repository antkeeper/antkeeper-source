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

#ifndef ANTKEEPER_GEOM_PRIMITIVES_LINE_SEGMENT_HPP
#define ANTKEEPER_GEOM_PRIMITIVES_LINE_SEGMENT_HPP

#include <engine/math/vector.hpp>

namespace geom {
namespace primitives {

/**
 * *n*-dimensional line segment.
 *
 * @tparam T Real type.
 * @tparam N Number of dimensions.
 */
template <class T, std::size_t N>
struct line_segment
{
	/// Vector type.
	using vector_type = math::vector<T, N>;
	
	/// First endpoint.
	vector_type a;
	
	/// Second endpoint.
	vector_type b;
	
	/**
	 * Calculates the square length of the line segment.
	 *
	 * @return Square length of the line segment.
	 */
	[[nodiscard]] inline T sqr_length() const noexcept
	{
		return math::sqr_distance(a, b);
	}
	
	/**
	 * Calculates the length of the line segment.
	 *
	 * @return Length of the line segment.
	 */
	[[nodiscard]] inline T length() const noexcept
	{
		const T d = sqr_length();
		return d ? std::sqrt(d) : d;
	}
	
	/**
	 * Calculates the square distance from the line segment to a point.
	 *
	 * @param point Input point.
	 *
	 * @return Square distance from the line segment to @p point.
	 */
	[[nodiscard]] T sqr_distance(const vector_type& point) const noexcept
	{
		const vector_type ab = b - a;
		const vector_type ap = point - a;
		
		const T t = math::dot(ap, ab);
		if (t <= T{0})
		{
			return math::sqr_length(ap);
		}
		
		const T ab_sqr_length = math::sqr_length(ab);
		if (t >= ab_sqr_length)
		{
			return math::sqr_length(point - b);
		}
		
		return math::sqr_length(ap) - (t * t) / ab_sqr_length;
	}
	
	/**
	 * Calculates the distance from the line segment to a point.
	 *
	 * @param point Input point.
	 *
	 * @return Distance from the line segment to @p point.
	 */
	[[nodiscard]] inline T distance(const vector_type& point) const
	{
		const T d = sqr_distance(point);
		return d ? std::sqrt(d) : d;
	}
};

} // namespace primitives

using namespace primitives;

} // namespace geom

#endif // ANTKEEPER_GEOM_PRIMITIVES_LINE_SEGMENT_HPP

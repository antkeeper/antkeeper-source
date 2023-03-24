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

#ifndef ANTKEEPER_GEOM_PRIMITIVES_HYPERCAPSULE_HPP
#define ANTKEEPER_GEOM_PRIMITIVES_HYPERCAPSULE_HPP

#include <engine/geom/primitives/line-segment.hpp>
#include <engine/math/vector.hpp>

namespace geom {
namespace primitives {

/**
 * *n*-dimensional capsule.
 *
 * @tparam T Real type.
 * @tparam N Number of dimensions.
 */
template <class T, std::size_t N>
struct hypercapsule
{
	/// Vector type.
	using vector_type = math::vector<T, N>;
	
	/// Line segment type.
	using segment_type = geom::line_segment<T, N>;
	
	/// Medial line segment.
	segment_type segment;
	
	/// Radius of the hemi-hyperspheres.
	T radius;
	
	/**
	 * Tests whether a point is contained within this hypercapsule.
	 *
	 * @param point Point to test for containment.
	 *
	 * @return `true` if the point is contained within this hypercapsule, `false` otherwise.
	 */
	[[nodiscard]] inline constexpr bool contains(const vector_type& point) const noexcept
	{
		return segment.sqr_distance(point) <= radius * radius;
	}
	
	/**
	 * Calculates the signed distance from the hypercapsule to a point.
	 *
	 * @param point Input point.
	 *
	 * @return Signed distance from the hypercapsule to @p point.
	 */
	[[nodiscard]] inline T distance(const vector_type& point) const noexcept
	{
		const T d = segment.sqr_distance(point);
		return (d ? std::sqrt(d) : d) - radius;
	}
};

} // namespace primitives

using namespace primitives;

} // namespace geom

#endif // ANTKEEPER_GEOM_PRIMITIVES_HYPERCAPSULE_HPP

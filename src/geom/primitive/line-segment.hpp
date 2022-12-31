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

#ifndef ANTKEEPER_GEOM_PRIMITIVE_LINE_SEGMENT_HPP
#define ANTKEEPER_GEOM_PRIMITIVE_LINE_SEGMENT_HPP

#include "math/vector.hpp"

namespace geom {
namespace primitive {

/**
 * *n*-dimensional line segment.
 *
 * @tparam T Real type.
 * @tparam N Number of dimensions.
 */
template <class T, std::size_t N>
struct line_segment
{
	typedef math::vector<T, N> vector_type;
	
	/// First endpoint.
	vector_type a;
	
	/// Second endpoint.
	vector_type b;
	
	/**
	 * Calculates the length of the line segment.
	 *
	 * @return Length of the line segment.
	 */
	T length() const noexcept
	{
		return math::distance(a, b);
	}
};

} // namespace primitive
} // namespace geom

#endif // ANTKEEPER_GEOM_PRIMITIVE_LINE_SEGMENT_HPP

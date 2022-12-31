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

#ifndef ANTKEEPER_GEOM_PRIMITIVE_HYPERPLANE_HPP
#define ANTKEEPER_GEOM_PRIMITIVE_HYPERPLANE_HPP

#include "math/vector.hpp"

namespace geom {
namespace primitive {

/**
 * *n*-dimensional plane.
 *
 * @tparam T Real type.
 * @tparam N Number of dimensions.
 */
template <class T, std::size_t N>
struct hyperplane
{
	typedef math::vector<T, N> vector_type;
	
	/// Hyperplane normal.
	vector_type normal;
	
	/// Hyperplane constant.
	T constant;
	
	/// Constructs a hyperplane.
	constexpr hyperplane() noexcept = default;
	
	/**
	 * Constructs a hyperplane given a point and a normal.
	 *
	 * @param point Point.
	 * @param normal Normal.
	 */
	constexpr hyperplane(const vector_type& point, const vector_type& normal) noexcept:
		normal(normal),
		constant(-math::dot(normal, point))
	{}
	
	/**
	 * Calculates the signed distance from the hyperplane to a point.
	 *
	 * @param point Input point.
	 *
	 * @return Signed distance from the hyperplane to @p point.
	 */
	constexpr T distance(const vector_type& point) const noexcept
	{
		return math::dot(normal, point) + constant;
	}
};

} // namespace primitive
} // namespace geom

#endif // ANTKEEPER_GEOM_PRIMITIVE_HYPERPLANE_HPP

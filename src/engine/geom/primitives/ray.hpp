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

#ifndef ANTKEEPER_GEOM_PRIMITIVES_RAY_HPP
#define ANTKEEPER_GEOM_PRIMITIVES_RAY_HPP

#include <engine/math/vector.hpp>
#include <algorithm>
#include <cmath>

namespace geom {
namespace primitives {

/**
 * Half of a line proceeding from an initial point.
 *
 * @tparam T Real type.
 * @tparam N Number of dimensions.
 */
template <class T, std::size_t N>
struct ray
{
	/// Vector type.
	using vector_type = math::vector<T, N>;
	
	/// Ray origin position.
	vector_type origin;
	
	/// Ray direction vector.
	vector_type direction;
	
	/**
	 * Extrapolates from the ray origin along the ray direction vector.
	 *
	 * @param distance Signed extrapolation distance.
	 *
	 * @return Extrapolated coordinates.
	 */
	[[nodiscard]] inline constexpr vector_type extrapolate(T distance) const noexcept
	{
		return origin + direction * distance;
	}
	
	/**
	 * Calculates the closest point on the ray to a point.
	 *
	 * @param point Input point.
	 *
	 * @return Closest point on the ray to @p point.
	 */
	[[nodiscard]] inline constexpr vector_type closest_point(const vector_type& point) const noexcept
	{
		return extrapolate(std::max<T>(T{0}, math::dot(point - origin, direction)));
	}
	
	/**
	 * Calculates the square distance from the ray to a point.
	 *
	 * @param point Input point.
	 *
	 * @return Square distance from the ray to @p point.
	 */
	[[nodiscard]] inline constexpr T sqr_distance(const vector_type& point) const noexcept
	{
		return math::sqr_distance(point, closest_point(point));
	}
	
	/**
	 * Calculates the distance from the ray to a point.
	 *
	 * @param point Input point.
	 *
	 * @return Distance from the ray to @p point.
	 */
	[[nodiscard]] inline T distance(const vector_type& point) const noexcept
	{
		const T d = sqr_distance(point);
		return (d) ? std::sqrt(d) : d;
	}
};

} // namespace primitives

using namespace primitives;

} // namespace geom

#endif // ANTKEEPER_GEOM_PRIMITIVES_RAY_HPP

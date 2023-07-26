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

#ifndef ANTKEEPER_GEOM_CLOSEST_FEATURE_HPP
#define ANTKEEPER_GEOM_CLOSEST_FEATURE_HPP

#include <engine/geom/primitives/point.hpp>
#include <engine/geom/primitives/triangle.hpp>
#include <algorithm>
#include <tuple>

namespace geom {

/**
 * Calculates the closest features on a triangle to a point.
 *
 * @tparam T Real type.
 *
 * @param tri Triangle.
 * @param a First point of triangle.
 * @param b Second point of triangle.
 * @param c Third point of triangle.
 * @param p Point.
 *
 * @return Tuple containing the Barycentric coordinates of the closest point on the triangle to point @p p, followed by the index of the edge on which the point lies (`-1` if not on an edge).
 */
/// @{
template <class T>
[[nodiscard]] constexpr std::tuple<point<T, 3>, int> closest_feature(const point<T, 3>& a, const point<T, 3>& b, const point<T, 3>& c, const point<T, 3>& p) noexcept
{
	const auto ab = b - a;
	const auto ca = a - c;
	const auto ap = p - a;
	const auto n = math::cross(ab, ca);
	const auto d = math::sqr_length(n);
	const auto q = math::cross(n, ap);
	
	point<T, 3> uvw;
	if ((uvw.z() = math::dot(q, ab) / d) < T{0})
	{
		uvw.z() = T{0};
		uvw.y() = std::min<T>(std::max<T>(math::dot(ab, ap) / math::sqr_length(ab), T{0}), T{1});
		uvw.x() = T{1} - uvw.y();
		return {uvw, 0};
	}
	else if ((uvw.y() = math::dot(q, ca) / d) < T{0})
	{
		uvw.y() = T{0};
		uvw.x() = std::min<T>(std::max<T>(math::dot(ca, p - c) / math::sqr_length(ca), T{0}), T{1});
		uvw.z() = T{1} - uvw.x();
		return {uvw, 2};
	}
	else if ((uvw.x() = T{1} - uvw.y() - uvw.z()) < T{0})
	{
		uvw.x() = T{0};
		const auto bc = c - b;
		uvw.z() = std::min<T>(std::max<T>(math::dot(bc, p - b) / math::sqr_length(bc), T{0}), T{1});
		uvw.y() = T{1} - uvw.z();
		return {uvw, 1};
	}
	
	return {uvw, -1};
}

template <class T>
[[nodiscard]] inline constexpr std::tuple<point<T, 3>, int> closest_feature(const triangle<T, 3>& tri, const point<T, 3>& p) noexcept
{
	return closest_feature(tri.a, tri.b, tri.c, p);
}
/// @}

} // namespace geom

#endif // ANTKEEPER_GEOM_CLOSEST_FEATURE_HPP

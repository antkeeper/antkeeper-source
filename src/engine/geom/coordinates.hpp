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

#ifndef ANTKEEPER_GEOM_COORDINATES_HPP
#define ANTKEEPER_GEOM_COORDINATES_HPP

#include <engine/geom/primitives/point.hpp>
#include <cmath>

namespace geom {

/**
 * Converts Cartesian coordinates to barycentric coordinates.
 *
 * @tparam T Real type.
 *
 * @param p Barycentric coordinates of point to convert.
 * @param a Cartesian coordinates of first point of triangle.
 * @param b Cartesian coordinates of second point of triangle.
 * @param c Cartesian coordinates of third point of triangle.
 *
 * @return Cartesian coordinates of point @p p.
 */
template <class T>
[[nodiscard]] inline constexpr point<T, 3> barycentric_to_cartesian(const point<T, 3>& p, const point<T, 3>& a, const point<T, 3>& b, const point<T, 3>& c) noexcept
{
	return a * p.x() + b * p.y() + c * p.z();
}

/**
 * Converts Cartesian coordinates to barycentric coordinates.
 *
 * @tparam T Real type.
 *
 * @param p Cartesian coordinates of point to convert.
 * @param a Cartesian coordinates of first point of triangle.
 * @param b Cartesian coordinates of second point of triangle.
 * @param c Cartesian coordinates of third point of triangle.
 *
 * @return Barycentric coordinates of point @p p.
 */
template <class T>
[[nodiscard]] constexpr point<T, 3> cartesian_to_barycentric(const point<T, 3>& p, const point<T, 3>& a, const point<T, 3>& b, const point<T, 3>& c) noexcept
{
	const auto ab = b - a;
	const auto ca = a - c;
	const auto n = math::cross(ab, ca);
	const auto d = math::sqr_length(n);
	const auto q = math::cross(n, p - a);
	
	point<T, 3> uvw;
	uvw.z() = math::dot(q, ab) / d;
	uvw.y() = math::dot(q, ca) / d;
	uvw.x() = T{1} - uvw.y() - uvw.z();
	
	return uvw;
}

/**
 * Converts Cartesian (rectangular) coordinates to spherical coordinates.
 *
 * @tparam T Real type.
 *
 * @param p Cartesian coordinates of point to convert.
 *
 * @return Spherical coordinates of point @p p, in the ISO order of radial distance, polar angle (radians), and azimuthal angle (radians).
 */
template <class T>
[[nodiscard]] point<T, 3> cartesian_to_spherical(const point<T, 3>& p)
{
	const T xx_yy = p.x() * p.x() + p.y() * p.y();
	
	return
	{
		std::sqrt(xx_yy + p.z() * p.z()),
		std::atan2(p.z(), std::sqrt(xx_yy)),
		std::atan2(p.y(), p.x())
	};
}

/**
 * Converts spherical coordinates to Cartesian (rectangular) coordinates.
 *
 * @tparam T Real type.
 *
 * @param p Spherical coordinates to convert, in the ISO order of radial distance, polar angle (radians), and azimuthal angle (radians).
 *
 * @return Cartesian coordinates of point @p p.
 */
template <class T>
[[nodiscard]] point<T, 3> spherical_to_cartesian(const point<T, 3>& p)
{
	const T x = p.x() * std::cos(p.y());
	
	return
	{
		x * std::cos(p.z()),
		x * std::sin(p.z()),
		p.x() * std::sin(p.y())
	};
}

} // namespace geom

#endif // ANTKEEPER_GEOM_COORDINATES_HPP

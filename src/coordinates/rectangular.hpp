/*
 * Copyright (C) 2021  Christopher J. Howard
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

#ifndef ANTKEEPER_COORDINATES_RECTANGULAR_HPP
#define ANTKEEPER_COORDINATES_RECTANGULAR_HPP

#include "utility/fundamental-types.hpp"
#include <cmath>

namespace coordinates {

/// Rectangular (Cartesian) coordinate systems.
namespace rectangular {

/**
 * Produces a matrix which rotates rectangular coordinates about the x-axis by a given angle.
 *
 * @param angle Angle of rotation, in radians.
 * @return Rotation matrix.
 */
template <class T>
math::matrix3<T> rotate_x(T angle);

/**
 * Rotates rectangular coordinates about the x-axis.
 *
 * @param v Rectangular coordinates to rotate.
 * @param angle Angle of rotation, in radians.
 * @return Rotated rectangular coordinates.
 */
template <class T>
math::vector3<T> rotate_x(const math::vector3<T>& v, T angle);

/**
 * Produces a matrix which rotates rectangular coordinates about the y-axis by a given angle.
 *
 * @param angle Angle of rotation, in radians.
 * @return Rotation matrix.
 */
template <class T>
math::matrix3<T> rotate_y(T angle);

/**
 * Rotates rectangular coordinates about the y-axis.
 *
 * @param v Rectangular coordinates to rotate.
 * @param angle Angle of rotation, in radians.
 * @return Rotated rectangular coordinates.
 */
template <class T>
math::vector3<T> rotate_y(const math::vector3<T>& v, T angle);

/**
 * Produces a matrix which rotates rectangular coordinates about the z-axis by a given angle.
 *
 * @param angle Angle of rotation, in radians.
 * @return Rotation matrix.
 */
template <class T>
math::matrix3<T> rotate_z(T angle);

/**
 * Rotates rectangular coordinates about the z-axis.
 *
 * @param v Rectangular coordinates to rotate.
 * @param angle Angle of rotation, in radians.
 * @return Rotated rectangular coordinates.
 */
template <class T>
math::vector3<T> rotate_z(const math::vector3<T>& v, T angle);

/**
 * Converts rectangular coordinates to spherical coordinates.
 *
 * @param v Rectangular coordinates.
 * @return Spherical coordinates, in the ISO order of radial distance, polar angle (radians), and azimuthal angle (radians).
 *
 * @see coordinates::spherical
 */
template <class T>
math::vector3<T> to_spherical(const math::vector3<T>& v);
	
template <class T>
math::vector3<T> to_spherical(const math::vector3<T>& v)
{
	const T xx_yy = v.x * v.x + v.y * v.y;
	
	return math::vector3<T>
	{
		std::sqrt(xx_yy + v.z * v.z),
		std::atan2(v.z, std::sqrt(xx_yy)),
		std::atan2(v.y, v.x)
	};
}

template <class T>
math::matrix3<T> rotate_x(T angle)
{
	const T c = std::cos(angle);
	const T s = std::sin(angle);
	
	return math::matrix3<T>
	{
		T(1), T(0), T(0),
		T(0), c, -s,
		T(0), s, c
	};
}

template <class T>
math::vector3<T> rotate_x(const math::vector3<T>& v, T angle)
{
	return rotate_x(angle) * v;
}

template <class T>
math::matrix3<T> rotate_y(T angle)
{
	const T c = std::cos(angle);
	const T s = std::sin(angle);
	
	return math::matrix3<T>
	{
		c, T(0), s,
		T(0), T(1), T(0),
		-s, T(0), c
	};
}

template <class T>
math::vector3<T> rotate_y(const math::vector3<T>& v, T angle)
{
	return rotate_y(angle) * v;
}

template <class T>
math::matrix3<T> rotate_z(T angle)
{
	const T c = std::cos(angle);
	const T s = std::sin(angle);
	
	return math::matrix3<T>
	{
		c, -s, T(0),
		s, c, T(0),
		T(0), T(0), T(1)
	};
}

template <class T>
math::vector3<T> rotate_z(const math::vector3<T>& v, T angle)
{
	return rotate_z(angle) * v;
}

} // namespace rectangular
} // namespace coordinates

#endif // ANTKEEPER_COORDINATES_RECTANGULAR_HPP

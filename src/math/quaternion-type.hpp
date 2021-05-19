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

#ifndef ANTKEEPER_MATH_QUATERNION_TYPE_HPP
#define ANTKEEPER_MATH_QUATERNION_TYPE_HPP

#include <cmath>

namespace math {

/**
 * A quaternion type is a tuple made of a scalar (real) part and vector (imaginary) part.
 *
 * @tparam T Scalar type.
 */
template <class T>
struct quaternion
{
	typedef T scalar_type;
	scalar_type w;
	scalar_type x;
	scalar_type y;
	scalar_type z;
	
	/// Returns the rotation identity quaternion.
	static constexpr quaternion identity();
	
	/**
	 * Returns a quaternion representing a rotation about the x-axis.
	 *
	 * @param angle Angle of rotation, in radians.
	 * @return Quaternion representing an x-axis rotation.
	 */
	static quaternion rotate_x(scalar_type angle);
	
	/**
	 * Returns a quaternion representing a rotation about the y-axis.
	 *
	 * @param angle Angle of rotation, in radians.
	 * @return Quaternion representing an y-axis rotation.
	 */
	static quaternion rotate_y(scalar_type angle);
	
	/**
	 * Returns a quaternion representing a rotation about the z-axis.
	 *
	 * @param angle Angle of rotation, in radians.
	 * @return Quaternion representing an z-axis rotation.
	 */
	static quaternion rotate_z(scalar_type angle);
};

template <class T>
constexpr quaternion<T> quaternion<T>::identity()
{
	return {T(1), T(0), T(0), T(0)};
};

template <class T>
quaternion<T> quaternion<T>::rotate_x(scalar_type angle)
{
	return {std::cos(angle * T(0.5)), std::sin(angle * T(0.5)), T(0), T(0)};
}

template <class T>
quaternion<T> quaternion<T>::rotate_y(scalar_type angle)
{
	return {std::cos(angle * T(0.5)), T(0), std::sin(angle * T(0.5)), T(0)};
}

template <class T>
quaternion<T> quaternion<T>::rotate_z(scalar_type angle)
{
	return {std::cos(angle * T(0.5)), T(0), T(0), std::sin(angle * T(0.5))};
}

} // namespace math

#endif // ANTKEEPER_MATH_QUATERNION_TYPE_HPP

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

#ifndef ANTKEEPER_MATH_QUATERNION_FUNCTIONS_HPP
#define ANTKEEPER_MATH_QUATERNION_FUNCTIONS_HPP

#include "math/matrix-type.hpp"
#include "math/quaternion-type.hpp"
#include "math/vector-type.hpp"
#include "math/vector-functions.hpp"
#include <cmath>

namespace math {

/**
 * Adds two quaternions.
 *
 * @param x First quaternion.
 * @param y Second quaternion.
 * @return Sum of the two quaternions.
 */
template <class T>
quaternion<T> add(const quaternion<T>& x, const quaternion<T>& y);

/**
 * Calculates the conjugate of a quaternion.
 *
 * @param x Quaternion from which the conjugate will be calculated.
 * @return Conjugate of the quaternion.
 */
template <class T>
quaternion<T> conjugate(const quaternion<T>& x);

/**
 * Calculates the dot product of two quaternions.
 *
 * @param x First quaternion.
 * @param y Second quaternion.
 * @return Dot product of the two quaternions.
 */
template <class T>
T dot(const quaternion<T>& x, const quaternion<T>& y);

/**
 * Divides a quaternion by a scalar.
 *
 * @param q Quaternion.
 * @param s Scalar.
 * @return Result of the division.
 */
template <class T>
quaternion<T> div(const quaternion<T>& q, T s);

/**
 * Calculates the length of a quaternion.
 * 
 * @param x Quaternion to calculate the length of.
 * @return Length of the quaternion.
 */
template <class T>
T length(const quaternion<T>& x);

/**
 * Calculates the squared length of a quaternion. The squared length can be calculated faster than the length because a call to `std::sqrt` is saved.
 * 
 * @param x Quaternion to calculate the squared length of.
 * @return Squared length of the quaternion.
 */
template <class T>
T length_squared(const quaternion<T>& x);

/**
 * Performs linear interpolation between two quaternions.
 *
 * @param x First quaternion.
 * @param y Second quaternion.
 * @param a Interpolation factor.
 * @return Interpolated quaternion.
 */
template <class T>
quaternion<T> lerp(const quaternion<T>& x, const quaternion<T>& y, T a);

/**
 * Creates a unit quaternion rotation using forward and up vectors.
 *
 * @param forward Unit forward vector.
 * @param up Unit up vector.
 * @return Unit rotation quaternion.
 */
template <class T>
quaternion<T> look_rotation(const vector<T, 3>& forward, vector<T, 3> up);

/**
 * Converts a quaternion to a rotation matrix.
 *
 * @param q Unit quaternion.
 * @return Matrix representing the rotation described by `q`.
 */
template <class T>
matrix<T, 3, 3> matrix_cast(const quaternion<T>& q);

/**
 * Multiplies two quaternions.
 *
 * @param x First quaternion.
 * @param y Second quaternion.
 * @return Product of the two quaternions.
 */
template <class T>
quaternion<T> mul(const quaternion<T>& x, const quaternion<T>& y);

/**
 * Multiplies a quaternion by a scalar.
 *
 * @param q Quaternion.
 * @param s Scalar.
 * @return Product of the quaternion and scalar.
 */
template <class T>
quaternion<T> mul(const quaternion<T>& q, T s);

/**
 * Rotates a 3-dimensional vector by a quaternion.
 *
 * @param q Unit quaternion.
 * @param v Vector.
 * @return Rotated vector.
 */
template <class T>
vector<T, 3> mul(const quaternion<T>& q, const vector<T, 3>& v);

/**
 * Negates a quaternion.
 */
template <class T>
quaternion<T> negate(const quaternion<T>& x);

/**
 * Performs normalized linear interpolation between two quaternions.
 *
 * @param x First quaternion.
 * @param y Second quaternion.
 * @param a Interpolation factor.
 * @return Interpolated quaternion.
 */
template <class T>
quaternion<T> nlerp(const quaternion<T>& x, const quaternion<T>& y, T a);

/**
 * Normalizes a quaternion.
 */
template <class T>
quaternion<T> normalize(const quaternion<T>& x);

/**
 * Creates a rotation from an angle and axis.
 *
 * @param angle Angle of rotation (in radians).
 * @param axis Axis of rotation
 * @return Quaternion representing the rotation.
 */
template <class T>
quaternion<T> angle_axis(T angle, const vector<T, 3>& axis);

/**
 * Calculates the minimum rotation between two normalized direction vectors.
 *
 * @param source Normalized source direction.
 * @param destination Normalized destination direction.
 * @return Quaternion representing the minimum rotation between the source and destination vectors.
 */
template <class T>
quaternion<T> rotation(const vector<T, 3>& source, const vector<T, 3>& destination);

/**
 * Performs spherical linear interpolation between two quaternions.
 *
 * @param x First quaternion.
 * @param y Second quaternion.
 * @param a Interpolation factor.
 * @return Interpolated quaternion.
 */
template <class T>
quaternion<T> slerp(const quaternion<T>& x, const quaternion<T>& y, T a);

/**
 * Subtracts a quaternion from another quaternion.
 *
 * @param x First quaternion.
 * @param y Second quaternion.
 * @return Difference between the quaternions.
 */
template <class T>
quaternion<T> sub(const quaternion<T>& x, const quaternion<T>& y);

/**
 * Converts a 3x3 rotation matrix to a quaternion.
 *
 * @param m Rotation matrix.
 * @return Unit quaternion representing the rotation described by `m`.
 */
template <class T>
quaternion<T> quaternion_cast(const matrix<T, 3, 3>& m);

/**
 * Types casts each quaternion component and returns a quaternion of the casted type.
 *
 * @tparam T2 Target quaternion component type.
 * @tparam T1 Source quaternion component type.
 * @param q Quaternion to type cast.
 * @return Type-casted quaternion.
 */
template <class T2, class T1>
quaternion<T2> type_cast(const quaternion<T1>& q);

template <class T>
inline quaternion<T> add(const quaternion<T>& x, const quaternion<T>& y)
{
	return {x.w + y.w, x.x + y.x, x.y + y.y, x.z + y.z};
}

template <class T>
inline quaternion<T> conjugate(const quaternion<T>& x)
{
	return {x.w, -x.x, -x.y, -x.z};
}

template <class T>
inline T dot(const quaternion<T>& x, const quaternion<T>& y)
{
	return {x.w * y.w + x.x * y.x + x.y * y.y + x.z * y.z};
}

template <class T>
inline quaternion<T> div(const quaternion<T>& q, T s)
{
	return {q.w / s, q.x / s, q.y / s, q.z / s}
}

template <class T>
inline T length(const quaternion<T>& x)
{
	return std::sqrt(x.w * x.w + x.x * x.x + x.y * x.y + x.z * x.z);
}

template <class T>
inline T length_squared(const quaternion<T>& x)
{
	return x.w * x.w + x.x * x.x + x.y * x.y + x.z * x.z;
}

template <class T>
inline quaternion<T> lerp(const quaternion<T>& x, const quaternion<T>& y, T a)
{
	return
		{
			(y.w - x.w) * a + x.w,
			(y.x - x.x) * a + x.x,
			(y.y - x.y) * a + x.y,
			(y.z - x.z) * a + x.z
		};
}

template <class T>
quaternion<T> look_rotation(const vector<T, 3>& forward, vector<T, 3> up)
{
	vector<T, 3> right = normalize(cross(forward, up));
	up = cross(right, forward);

	matrix<T, 3, 3> m =
		{{
			{right[0], up[0], -forward[0]},
			{right[1], up[1], -forward[1]},
			{right[2], up[2], -forward[2]}
		}};

	// Convert to quaternion
	return normalize(quaternion_cast(m));
}

template <class T>
matrix<T, 3, 3> matrix_cast(const quaternion<T>& q)
{
	T wx = q.w * q.x;
	T wy = q.w * q.y;
	T wz = q.w * q.z;
	T xx = q.x * q.x;
	T xy = q.x * q.y;
	T xz = q.x * q.z;
	T yy = q.y * q.y;
	T yz = q.y * q.z;
	T zz = q.z * q.z;

	return
		{{
			{T(1) - (yy + zz) * T(2), (xy + wz) * T(2), (xz - wy) * T(2)},
			{(xy - wz) * T(2), T(1) - (xx + zz) * T(2), (yz + wx) * T(2)},
			{(xz + wy) * T(2), (yz - wx) * T(2), T(1) - (xx + yy) * T(2)}
		}};
}

template <class T>
quaternion<T> mul(const quaternion<T>& x, const quaternion<T>& y)
{
	return
		{
			-x.x * y.x - x.y * y.y - x.z * y.z + x.w * y.w,
			 x.x * y.w + x.y * y.z - x.z * y.y + x.w * y.x,
			-x.x * y.z + x.y * y.w + x.z * y.x + x.w * y.y,
			 x.x * y.y - x.y * y.x + x.z * y.w + x.w * y.z
		};
}

template <class T>
inline quaternion<T> mul(const quaternion<T>& q, T s)
{
	return {q.w * s, q.x * s, q.y * s, q.z * s};
}

template <class T>
vector<T, 3> mul(const quaternion<T>& q, const vector<T, 3>& v)
{
	const vector<T, 3>& i = reinterpret_cast<const vector<T, 3>&>(q.x);	
	return add(add(mul(i, dot(i, v) * T(2)), mul(v, (q.w * q.w - dot(i, i)))), mul(cross(i, v), q.w * T(2)));
}

template <class T>
inline quaternion<T> negate(const quaternion<T>& x)
{
	return {-x.w, -x.x, -x.y, -x.z};
}

template <class T>
quaternion<T> nlerp(const quaternion<T>& x, const quaternion<T>& y, T a)
{
	if (dot(x, y) < T(0))
	{
		return normalize(add(mul(x, T(1) - a), mul(y, -a)));
	}
	else
	{
		return normalize(add(mul(x, T(1) - a), mul(y, a)));
	}
}

template <class T>
inline quaternion<T> normalize(const quaternion<T>& x)
{
	return mul(x, T(1) / length(x));
}

template <class T>
quaternion<T> angle_axis(T angle, const vector<T, 3>& axis)
{
	T s = std::sin(angle * T(0.5));
	return {static_cast<T>(std::cos(angle * T(0.5))), axis.x * s, axis.y * s, axis.z * s};
}

template <class T>
quaternion<T> rotation(const vector<T, 3>& source, const vector<T, 3>& destination)
{
	quaternion<T> q;
	q.w = dot(source, destination);
	reinterpret_cast<vector<T, 3>&>(q.x) = cross(source, destination);

	q.w += length(q);
	return normalize(q);
}

template <class T>
quaternion<T> slerp(const quaternion<T>& x, const quaternion<T>& y, T a)
{
	T cos_theta = dot(x, y);

	constexpr T epsilon = T(0.0005);
	if (cos_theta > T(1) - epsilon)
	{
		return normalize(lerp(x, y, a));
	}

	cos_theta = std::max<T>(T(-1), std::min<T>(T(1), cos_theta));
	T theta = static_cast<T>(std::acos(cos_theta)) * a;

	quaternion<T> z = normalize(sub(y, mul(x, cos_theta)));

	return add(mul(x, static_cast<T>(std::cos(theta))), mul(z, static_cast<T>(std::sin(theta))));
}

template <class T>
inline quaternion<T> sub(const quaternion<T>& x, const quaternion<T>& y)
{
	return {x.w - y.w, x.x - y.x, x.y - y.y, x.z - y.z};
}

template <class T>
quaternion<T> quaternion_cast(const matrix<T, 3, 3>& m)
{
	T r;
	vector<T, 3> i;

	T trace = m[0][0] + m[1][1] + m[2][2];
	if (trace > T(0))
	{
		T s = T(0.5) / std::sqrt(trace + T(1));
		r = T(0.25) / s;
		i =
			{
				(m[2][1] - m[1][2]) * s,
				(m[0][2] - m[2][0]) * s,
				(m[1][0] - m[0][1]) * s
			};
	}
	else
	{
		if (m[0][0] > m[1][1] && m[0][0] > m[2][2])
		{
			T s = T(2) * std::sqrt(T(1) + m[0][0] - m[1][1] - m[2][2]);
			r = (m[2][1] - m[1][2]) / s;
			i =
				{
					T(0.25) * s,
					(m[0][1] + m[1][0]) / s,
					(m[0][2] + m[2][0]) / s
				};
		}
		else if (m[1][1] > m[2][2])
		{
			T s = T(2) * std::sqrt(T(1) + m[1][1] - m[0][0] - m[2][2]);
			r = (m[0][2] - m[2][0]) / s;
			i =
				{
					(m[0][1] + m[1][0]) / s,
					T(0.25) * s,
					(m[1][2] + m[2][1]) / s
				};
		}
		else
		{
			T s = T(2) * std::sqrt(T(1) + m[2][2] - m[0][0] - m[1][1]);
			r = (m[1][0] - m[0][1]) / s;
			i = 
				{
					(m[0][2] + m[2][0]) / s,
					(m[1][2] + m[2][1]) / s,
					T(0.25) * s
				};
		}
	}

	return {r, i.x, i.y, i.z};
}

template <class T2, class T1>
inline quaternion<T2> type_cast(const quaternion<T1>& q)
{
	return quaternion<T2>
	{
		static_cast<T2>(q.w),
		static_cast<T2>(q.x),
		static_cast<T2>(q.y),
		static_cast<T2>(q.z)
	};
}

} // namespace math

#endif // ANTKEEPER_MATH_QUATERNION_FUNCTIONS_HPP


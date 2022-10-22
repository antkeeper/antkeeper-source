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

#ifndef ANTKEEPER_MATH_QUATERNION_HPP
#define ANTKEEPER_MATH_QUATERNION_HPP

#include "math/constants.hpp"
#include "math/matrix.hpp"
#include "math/vector.hpp"
#include <cmath>
#include <istream>
#include <ostream>

namespace math {

/**
 * Quaternion composed of a real scalar part and imaginary vector part.
 *
 * @tparam T Scalar type.
 */
template <class T>
struct quaternion
{
	/// Scalar type.
	typedef T scalar_type;
	
	/// Vector type.
	typedef vector<T, 3> vector_type;
	
	/// Rotation matrix type.
	typedef matrix<T, 3, 3> matrix_type;
	
	/// Quaternion real part.
	scalar_type r;
	
	/// Quaternion imaginary part.
	vector_type i;
	
	/// Returns a reference to the quaternion real part.
	/// @{
	constexpr inline scalar_type& w() noexcept
	{
		return r;
	}
	constexpr inline const scalar_type& w() const noexcept
	{
		return r;
	}
	/// @}
	
	/// Returns a reference to the first element of the quaternion imaginary part.
	/// @{
	constexpr inline scalar_type& x() noexcept
	{
		return i.x();
	}
	constexpr inline const scalar_type& x() const noexcept
	{
		return i.x();
	}
	/// @}
	
	/// Returns a reference to the second element of the quaternion imaginary part.
	/// @{
	constexpr inline scalar_type& y() noexcept
	{
		return i.y();
	}
	constexpr inline const scalar_type& y() const noexcept
	{
		return i.y();
	}
	/// @}
	
	/// Returns a reference to the third element of the quaternion imaginary part.
	/// @{
	constexpr inline scalar_type& z() noexcept
	{
		return i.z();
	}
	constexpr inline const scalar_type& z() const noexcept
	{
		return i.z();
	}
	/// @}
	
	/**
	 * Returns a quaternion representing a rotation about the x-axis.
	 *
	 * @param angle Angle of rotation, in radians.
	 *
	 * @return Quaternion representing an x-axis rotation.
	 */
	static quaternion rotate_x(scalar_type angle)
	{
		return {std::cos(angle * T(0.5)), std::sin(angle * T(0.5)), T(0), T(0)};
	}
	
	/**
	 * Returns a quaternion representing a rotation about the y-axis.
	 *
	 * @param angle Angle of rotation, in radians.
	 *
	 * @return Quaternion representing an y-axis rotation.
	 */
	static quaternion rotate_y(scalar_type angle)
	{
		return {std::cos(angle * T(0.5)), T(0), std::sin(angle * T(0.5)), T(0)};
	}
	
	/**
	 * Returns a quaternion representing a rotation about the z-axis.
	 *
	 * @param angle Angle of rotation, in radians.
	 * @return Quaternion representing an z-axis rotation.
	 */
	static quaternion rotate_z(scalar_type angle)
	{
		return {std::cos(angle * T(0.5)), T(0), T(0), std::sin(angle * T(0.5))};
	}
	
	/**
	 * Type-casts the quaternion scalars using `static_cast`.
	 *
	 * @tparam U Target scalar type.
	 *
	 * @return Type-casted quaternion.
	 */
	template <class U>
	constexpr inline explicit operator quaternion<U>() const noexcept
	{
		return {static_cast<U>(r), vector<U, 3>(i)};
	}
	
	/**
	 * Constructs a matrix representing the rotation described the quaternion.
	 *
	 * @return Rotation matrix.
	 */
	constexpr explicit operator matrix_type() const noexcept
	{
		const T xx = x() * x();
		const T xy = x() * y();
		const T xz = x() * z();
		const T xw = x() * w();
		const T yy = y() * y();
		const T yz = y() * z();
		const T yw = y() * w();
		const T zz = z() * z();
		const T zw = z() * w();

		return
		{
			T(1) - (yy + zz) * T(2), (xy + zw) * T(2), (xz - yw) * T(2),
			(xy - zw) * T(2), T(1) - (xx + zz) * T(2), (yz + xw) * T(2),
			(xz + yw) * T(2), (yz - xw) * T(2), T(1) - (xx + yy) * T(2)
		};
	}
	
	/**
	 * Casts the quaternion to a 4-element vector, with the real part as the first element and the imaginary part as the following three elements.
	 *
	 * @return Vector containing the real and imaginary parts of the quaternion.
	 */
	constexpr inline explicit operator vector<T, 4>() const noexcept
	{
		return {r, i[0], i[1], i[2]};
	}
	
	/// Returns a zero quaternion, where every scalar is equal to zero.
	static constexpr quaternion zero() noexcept
	{
		return {};
	}
	
	/// Returns a rotation identity quaternion.
	static constexpr quaternion identity() noexcept
	{
		return {T{1}, vector_type::zero()};
	}
};

/**
 * Adds two quaternions.
 *
 * @param a First quaternion.
 * @param b Second quaternion.
 *
 * @return Sum of the two quaternions.
 */
template <class T>
constexpr quaternion<T> add(const quaternion<T>& a, const quaternion<T>& b) noexcept;

/**
 * Adds a quaternion and a scalar.
 *
 * @param a First value.
 * @param b Second second value.
 *
 * @return Sum of the quaternion and scalar.
 */
template <class T>
constexpr quaternion<T> add(const quaternion<T>& a, T b) noexcept;

/**
 * Calculates the conjugate of a quaternion.
 *
 * @param q Quaternion from which the conjugate will be calculated.
 *
 * @return Conjugate of the quaternion.
 */
template <class T>
constexpr quaternion<T> conjugate(const quaternion<T>& q) noexcept;

/**
 * Calculates the dot product of two quaternions.
 *
 * @param a First quaternion.
 * @param b Second quaternion.
 *
 * @return Dot product of the two quaternions.
 */
template <class T>
constexpr T dot(const quaternion<T>& a, const quaternion<T>& b) noexcept;

/**
 * Divides a quaternion by another quaternion.
 *
 * @param a First value.
 * @param b Second value.
 *
 * @return Result of the division.
 */
template <class T>
constexpr quaternion<T> div(const quaternion<T>& a, const quaternion<T>& b) noexcept;

/**
 * Divides a quaternion by a scalar.
 *
 * @param a Quaternion.
 * @param b Scalar.
 *
 * @return Result of the division.
 */
template <class T>
constexpr quaternion<T> div(const quaternion<T>& a, T b) noexcept;

/**
 * Divides a scalar by a quaternion.
 *
 * @param a Scalar.
 * @param b Quaternion.
 *
 * @return Result of the division.
 */
template <class T>
constexpr quaternion<T> div(T a, const quaternion<T>& b) noexcept;

/**
 * Calculates the inverse length of a quaternion.
 * 
 * @param q Quaternion to calculate the inverse length of.
 *
 * @return Inverse length of the quaternion.
 */
template <class T>
T inv_length(const quaternion<T>& q);

/**
 * Calculates the length of a quaternion.
 * 
 * @param q Quaternion to calculate the length of.
 *
 * @return Length of the quaternion.
 */
template <class T>
T length(const quaternion<T>& q);

/**
 * Performs linear interpolation between two quaternions.
 *
 * @param a First quaternion.
 * @param b Second quaternion.
 * @param t Interpolation factor.
 *
 * @return Interpolated quaternion.
 */
template <class T>
constexpr quaternion<T> lerp(const quaternion<T>& a, const quaternion<T>& b, T t) noexcept;

/**
 * Creates a unit quaternion rotation using forward and up vectors.
 *
 * @param forward Unit forward vector.
 * @param up Unit up vector.
 *
 * @return Unit rotation quaternion.
 */
template <class T>
quaternion<T> look_rotation(const vector<T, 3>& forward, vector<T, 3> up);

/**
 * Multiplies two quaternions.
 *
 * @param a First quaternion.
 * @param b Second quaternion.
 *
 * @return Product of the two quaternions.
 */
template <class T>
constexpr quaternion<T> mul(const quaternion<T>& a, const quaternion<T>& b) noexcept;

/**
 * Multiplies a quaternion by a scalar.
 *
 * @param a First value.
 * @param b Second value.
 *
 * @return Product of the quaternion and scalar.
 */
template <class T>
constexpr quaternion<T> mul(const quaternion<T>& a, T b) noexcept;

/**
 * Calculates the product of a quaternion and a vector.
 *
 * @param a First value.
 * @param b second value.
 *
 * @return Product of the quaternion and vector.
 */
/// @{
template <class T>
constexpr vector<T, 3> mul(const quaternion<T>& a, const vector<T, 3>& b) noexcept;
template <class T>
constexpr vector<T, 3> mul(const vector<T, 3>& a, const quaternion<T>& b) noexcept;
/// @}

/**
 * Negates a quaternion.
 *
 * @param q Quaternion to negate.
 *
 * @return Negated quaternion.
 */
template <class T>
constexpr quaternion<T> negate(const quaternion<T>& q) noexcept;

/**
 * Performs normalized linear interpolation between two quaternions.
 *
 * @param a First quaternion.
 * @param b Second quaternion.
 * @param t Interpolation factor.
 *
 * @return Interpolated quaternion.
 */
template <class T>
quaternion<T> nlerp(const quaternion<T>& a, const quaternion<T>& b, T t);

/**
 * Normalizes a quaternion.
 *
 * @param q Quaternion to normalize.
 *
 * @return Normalized quaternion.
 */
template <class T>
quaternion<T> normalize(const quaternion<T>& q);

/**
 * Creates a rotation from an angle and axis.
 *
 * @param angle Angle of rotation (in radians).
 * @param axis Axis of rotation
 *
 * @return Quaternion representing the rotation.
 */
template <class T>
quaternion<T> angle_axis(T angle, const vector<T, 3>& axis);

/**
 * Calculates the minimum rotation between two normalized direction vectors.
 *
 * @param source Normalized source direction.
 * @param destination Normalized destination direction.
 *
 * @return Quaternion representing the minimum rotation between the source and destination vectors.
 */
template <class T>
quaternion<T> rotation(const vector<T, 3>& source, const vector<T, 3>& destination);

/**
 * Performs spherical linear interpolation between two quaternions.
 *
 * @param a First quaternion.
 * @param b Second quaternion.
 * @param t Interpolation factor.
 *
 * @return Interpolated quaternion.
 */
template <class T>
quaternion<T> slerp(const quaternion<T>& a, const quaternion<T>& b, T t, T error = T{1e-6});

/**
 * Calculates the square length of a quaternion. The square length can be calculated faster than the length because a call to `std::sqrt` is saved.
 * 
 * @param q Quaternion to calculate the square length of.
 *
 * @return Square length of the quaternion.
 */
template <class T>
constexpr T sqr_length(const quaternion<T>& q) noexcept;

/**
 * Subtracts a quaternion from another quaternion.
 *
 * @param a First quaternion.
 * @param b Second quaternion.
 *
 * @return Difference between the quaternions.
 */
template <class T>
constexpr quaternion<T> sub(const quaternion<T>& a, const quaternion<T>& b) noexcept;

/**
 * Subtracts a quaternion and a scalar.
 *
 * @param a First value.
 * @param b Second second.
 *
 * @return Difference between the quaternion and scalar.
 */
/// @{
template <class T>
constexpr quaternion<T> sub(const quaternion<T>& a, T b) noexcept;
template <class T>
constexpr quaternion<T> sub(T a, const quaternion<T>& b) noexcept;
/// @}

/**
 * Decomposes a quaternion into swing and twist rotation components.
 *
 * @param[in] q Quaternion to decompose.
 * @param[in] a Axis of twist rotation.
 * @param[out] swing Swing rotation component.
 * @param[out] twist Twist rotation component.
 * @param[in] error Threshold at which a number is considered zero.
 *
 * @see https://www.euclideanspace.com/maths/geometry/rotations/for/decomposition/
 */
template <class T>
void swing_twist(const quaternion<T>& q, const vector<T, 3>& a, quaternion<T>& qs, quaternion<T>& qt, T error = T{1e-6});

/**
 * Converts a 3x3 rotation matrix to a quaternion.
 *
 * @param m Rotation matrix.
 *
 * @return Unit quaternion representing the rotation described by @p m.
 */
template <class T>
quaternion<T> quaternion_cast(const matrix<T, 3, 3>& m);

template <class T>
constexpr inline quaternion<T> add(const quaternion<T>& a, const quaternion<T>& b) noexcept
{
	return {a.r + b.r, a.i + b.i};
}

template <class T>
constexpr inline quaternion<T> add(const quaternion<T>& a, T b) noexcept
{
	return {a.r + b, a.i + b};
}

template <class T>
constexpr inline quaternion<T> conjugate(const quaternion<T>& q) noexcept
{
	return {q.r, -q.i};
}

template <class T>
constexpr inline T dot(const quaternion<T>& a, const quaternion<T>& b) noexcept
{
	return a.r * b.r + dot(a.i, b.i);
}

template <class T>
constexpr inline quaternion<T> div(const quaternion<T>& a, const quaternion<T>& b) noexcept
{
	return {a.r / b.r, a.i / b.i};
}

template <class T>
constexpr inline quaternion<T> div(const quaternion<T>& a, T b) noexcept
{
	return {a.r / b, a.i / b};
}

template <class T>
constexpr inline quaternion<T> div(T a, const quaternion<T>& b) noexcept
{
	return {a / b.r, a / b.i};
}

template <class T>
inline T inv_length(const quaternion<T>& q)
{
	return T{1} / length(q);
}

template <class T>
inline T length(const quaternion<T>& q)
{
	return std::sqrt(sqr_length(q));
}

template <class T>
constexpr inline quaternion<T> lerp(const quaternion<T>& a, const quaternion<T>& b, T t) noexcept
{
	return
	{
		(b.r - a.r) * t + a.r,
		(b.i - a.i) * t + a.i
	};
}

template <class T>
quaternion<T> look_rotation(const vector<T, 3>& forward, vector<T, 3> up)
{
	vector<T, 3> right = normalize(cross(forward, up));
	up = cross(right, forward);

	matrix<T, 3, 3> m =
	{
		right,
		up,
		-forward
	};

	// Convert to quaternion
	return normalize(quaternion_cast(m));
}

template <class T>
constexpr quaternion<T> mul(const quaternion<T>& a, const quaternion<T>& b) noexcept
{
	return
		{
			-a.x() * b.x() - a.y() * b.y() - a.z() * b.z() + a.w() * b.w(),
			 a.x() * b.w() + a.y() * b.z() - a.z() * b.y() + a.w() * b.x(),
			-a.x() * b.z() + a.y() * b.w() + a.z() * b.x() + a.w() * b.y(),
			 a.x() * b.y() - a.y() * b.x() + a.z() * b.w() + a.w() * b.z()
		};
}

template <class T>
constexpr inline quaternion<T> mul(const quaternion<T>& a, T b) noexcept
{
	return {a.r * b, a.i * b};
}

template <class T>
constexpr vector<T, 3> mul(const quaternion<T>& a, const vector<T, 3>& b) noexcept
{
	return a.i * dot(a.i, b) * T(2) + b * (a.r * a.r - sqr_length(a.i)) + cross(a.i, b) * a.r * T(2);
}

template <class T>
constexpr inline vector<T, 3> mul(const vector<T, 3>& a, const quaternion<T>& b) noexcept
{
	return mul(conjugate(b), a);
}

template <class T>
constexpr inline quaternion<T> negate(const quaternion<T>& q) noexcept
{
	return {-q.r, -q.i};
}

template <class T>
quaternion<T> nlerp(const quaternion<T>& a, const quaternion<T>& b, T t)
{
	return normalize(add(mul(a, T(1) - t), mul(b, t * std::copysign(T(1), dot(a, b)))));
}

template <class T>
inline quaternion<T> normalize(const quaternion<T>& q)
{
	return mul(q, inv_length(q));
}

template <class T>
quaternion<T> angle_axis(T angle, const vector<T, 3>& axis)
{
	angle *= T{0.5};
	return {std::cos(angle), axis * std::sin(angle)};
}

template <class T>
quaternion<T> rotation(const vector<T, 3>& source, const vector<T, 3>& destination)
{
	quaternion<T> q = {dot(source, destination), cross(source, destination)};
	q.w() += length(q);
	return normalize(q);
}

template <class T>
quaternion<T> slerp(const quaternion<T>& a, const quaternion<T>& b, T t, T error)
{
	T cos_theta = dot(a, b);

	if (cos_theta > T(1) - error)
		return normalize(lerp(a, b, t));

	cos_theta = std::max<T>(T(-1), std::min<T>(T(1), cos_theta));
	const T theta = std::acos(cos_theta) * t;
	
	quaternion<T> c = normalize(sub(b, mul(a, cos_theta)));
	
	return add(mul(a, std::cos(theta)), mul(c, std::sin(theta)));
}

template <class T>
constexpr inline T sqr_length(const quaternion<T>& q) noexcept
{
	return q.r * q.r + sqr_length(q.i);
}

template <class T>
constexpr inline quaternion<T> sub(const quaternion<T>& a, const quaternion<T>& b) noexcept
{
	return {a.r - b.r, a.i - b.i};
}

template <class T>
constexpr inline quaternion<T> sub(const quaternion<T>& a, T b) noexcept
{
	return {a.r - b, a.i - b};
}

template <class T>
constexpr inline quaternion<T> sub(T a, const quaternion<T>& b) noexcept
{
	return {a - b.r, a - b.i};
}

template <class T>
void swing_twist(const quaternion<T>& q, const vector<T, 3>& a, quaternion<T>& qs, quaternion<T>& qt, T error)
{
	if (sqr_length(q.i) > error)
	{
		qt = normalize(quaternion<T>{q.w(), a * dot(a, q.i)});
		qs = mul(q, conjugate(qt));
	}
	else
	{
		qt = angle_axis(pi<T>, a);
		
		const vector<T, 3> qa = mul(q, a);
		const vector<T, 3> sa = cross(a, qa);
		if (sqr_length(sa) > error)
			qs = angle_axis(std::acos(dot(a, qa)), sa);
		else
			qs = quaternion<T>::identity();
	}
}

template <class T>
quaternion<T> quaternion_cast(const matrix<T, 3, 3>& m)
{
	const T t = trace(m);
	
	if (t > T(0))
	{
		T s = T(0.5) / std::sqrt(t + T(1));
		return
		{
			T(0.25) / s,
			(m[1][2] - m[2][1]) * s,
			(m[2][0] - m[0][2]) * s,
			(m[0][1] - m[1][0]) * s
		};
	}
	else
	{
		if (m[0][0] > m[1][1] && m[0][0] > m[2][2])
		{
			T s = T(2) * std::sqrt(T(1) + m[0][0] - m[1][1] - m[2][2]);
			
			return
			{
				(m[1][2] - m[2][1]) / s,
				T(0.25) * s,
				(m[1][0] + m[0][1]) / s,
				(m[2][0] + m[0][2]) / s
			};
		}
		else if (m[1][1] > m[2][2])
		{
			T s = T(2) * std::sqrt(T(1) + m[1][1] - m[0][0] - m[2][2]);
			return
			{
				(m[2][0] - m[0][2]) / s,
				(m[1][0] + m[0][1]) / s,
				T(0.25) * s,
				(m[2][1] + m[1][2]) / s
			};
		}
		else
		{
			T s = T(2) * std::sqrt(T(1) + m[2][2] - m[0][0] - m[1][1]);
			return
			{
				(m[0][1] - m[1][0]) / s,
				(m[2][0] + m[0][2]) / s,
				(m[2][1] + m[1][2]) / s,
				T(0.25) * s
			};
		}
	}
}

namespace operators {

/// @copydoc add(const quaternion<T>&, const quaternion<T>&)
template <class T>
constexpr inline quaternion<T> operator+(const quaternion<T>& a, const quaternion<T>& b) noexcept
{
	return add(a, b);
}

/// @copydoc add(const quaternion<T>&, T)
/// @{
template <class T>
constexpr inline quaternion<T> operator+(const quaternion<T>& a, T b) noexcept
{
	return add(a, b);
}
template <class T>
constexpr inline quaternion<T> operator+(T a, const quaternion<T>& b) noexcept
{
	return add(b, a);
}
/// @}

/// @copydoc div(const quaternion<T>&, const quaternion<T>&)
template <class T>
constexpr inline quaternion<T> operator/(const quaternion<T>& a, const quaternion<T>& b) noexcept
{
	return div(a, b);
}

/// @copydoc div(const quaternion<T>&, T)
template <class T>
constexpr inline quaternion<T> operator/(const quaternion<T>& a, T b) noexcept
{
	return div(a, b);
}

/// @copydoc div(T, const quaternion<T>&)
template <class T>
constexpr inline quaternion<T> operator/(T a, const quaternion<T>& b) noexcept
{
	return div(a, b);
}

/// @copydoc mul(const quaternion<T>&, const quaternion<T>&)
template <class T>
constexpr inline quaternion<T> operator*(const quaternion<T>& a, const quaternion<T>& b) noexcept
{
	return mul(a, b);
}

/// @copydoc mul(const quaternion<T>&, T)
/// @{
template <class T>
constexpr inline quaternion<T> operator*(const quaternion<T>& a, T b) noexcept
{
	return mul(a, b);
}
template <class T>
constexpr inline quaternion<T> operator*(T a, const quaternion<T>& b) noexcept
{
	return mul(b, a);
}
/// @}

/// @copydoc mul(const quaternion<T>&, const vector<T, 3>&)
template <class T>
constexpr inline vector<T, 3> operator*(const quaternion<T>& a, const vector<T, 3>& b) noexcept
{
	return mul(a, b);
}

/// @copydoc mul(const vector<T, 3>&, const quaternion<T>&)
template <class T>
constexpr inline vector<T, 3> operator*(const vector<T, 3>& a, const quaternion<T>& b) noexcept
{
	return mul(a, b);
}

/// @copydoc sub(const quaternion<T>&, const quaternion<T>&)
template <class T>
constexpr inline quaternion<T> operator-(const quaternion<T>& a, const quaternion<T>& b) noexcept
{
	return sub(a, b);
}

/// @copydoc sub(const quaternion<T>&, T)
/// @{
template <class T>
constexpr inline quaternion<T> operator-(const quaternion<T>& a, T b) noexcept
{
	return sub(a, b);
}
template <class T>
constexpr inline quaternion<T> operator-(T a, const quaternion<T>& b) noexcept
{
	return sub(a, b);
}
/// @}

/// @copydoc negate(const quaternion<T>&)
template <class T>
constexpr inline quaternion<T> operator-(const quaternion<T>& q) noexcept
{
	return negate(q);
}

/**
 * Adds two values and stores the result in the first value.
 *
 * @param a First value.
 * @param b Second value.
 *
 * @return Reference to the first value.
 */
/// @{
template <class T>
constexpr inline quaternion<T>& operator+=(quaternion<T>& a, const quaternion<T>& b) noexcept
{
	return (a = a + b);
}
template <class T>
constexpr inline quaternion<T>& operator+=(quaternion<T>& a, T b) noexcept
{
	return (a = a + b);
}
/// @}

/**
 * Subtracts the first value by the second value and stores the result in the first value.
 *
 * @param a First value.
 * @param b Second value.
 *
 * @return Reference to the first value.
 */
/// @{
template <class T>
constexpr inline quaternion<T>& operator-=(quaternion<T>& a, const quaternion<T>& b) noexcept
{
	return (a = a - b);
}
template <class T>
constexpr inline quaternion<T>& operator-=(quaternion<T>& a, T b) noexcept
{
	return (a = a - b);
}
/// @}

/**
 * Multiplies two values and stores the result in the first value.
 *
 * @param a First value.
 * @param b Second value.
 *
 * @return Reference to the first value.
 */
/// @{
template <class T>
constexpr inline quaternion<T>& operator*=(quaternion<T>& a, const quaternion<T>& b) noexcept
{
	return (a = a * b);
}
template <class T>
constexpr inline quaternion<T>& operator*=(quaternion<T>& a, T b) noexcept
{
	return (a = a * b);
}
/// @}

/**
 * Divides the first value by the second value and stores the result in the first value.
 *
 * @param a First value.
 * @param b Second value.
 *
 * @return Reference to the first value.
 */
/// @{
template <class T>
constexpr inline quaternion<T>& operator/=(quaternion<T>& a, const quaternion<T>& b) noexcept
{
	return (a = a / b);
}
template <class T>
constexpr inline quaternion<T>& operator/=(quaternion<T>& a, T b) noexcept
{
	return (a = a / b);
}
/// @}

/**
 * Writes the real and imaginary parts of a quaternion to an output stream, with each number delimeted by a space.
 *
 * @param os Output stream.
 * @param q Quaternion.
 *
 * @return Output stream.
 */
template <class T>
std::ostream& operator<<(std::ostream& os, const math::quaternion<T>& q)
{
	os << q.r << ' ' << q.i;
	return os;
}

/**
 * Reads the real and imaginary parts of a quaternion from an input stream, with each number delimeted by a space.
 *
 * @param is Input stream.
 * @param q Quaternion.
 *
 * @return Input stream.
 */
template <class T>
std::istream& operator>>(std::istream& is, const math::quaternion<T>& q)
{
	is >> q.r;
	is >> q.i;
	return is;
}

} // namespace operators

} // namespace math

using namespace math::operators;

#endif // ANTKEEPER_MATH_QUATERNION_HPP

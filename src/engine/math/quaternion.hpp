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

#ifndef ANTKEEPER_MATH_QUATERNION_HPP
#define ANTKEEPER_MATH_QUATERNION_HPP

#include <engine/math/numbers.hpp>
#include <engine/math/matrix.hpp>
#include <engine/math/vector.hpp>
#include <array>
#include <cmath>
#include <utility>

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
	using scalar_type = T;
	
	/// Vector type.
	using vector_type = vec3<T>;
	
	/// Rotation matrix type.
	using matrix_type = mat3<T>;
	
	/// Quaternion real part.
	scalar_type r;
	
	/// Quaternion imaginary part.
	vector_type i;
	
	/// Returns a reference to the quaternion real part.
	/// @{
	[[nodiscard]] inline constexpr scalar_type& w() noexcept
	{
		return r;
	}
	[[nodiscard]] inline constexpr const scalar_type& w() const noexcept
	{
		return r;
	}
	/// @}
	
	/// Returns a reference to the first element of the quaternion imaginary part.
	/// @{
	[[nodiscard]] inline constexpr scalar_type& x() noexcept
	{
		return i.x();
	}
	[[nodiscard]] inline constexpr const scalar_type& x() const noexcept
	{
		return i.x();
	}
	/// @}
	
	/// Returns a reference to the second element of the quaternion imaginary part.
	/// @{
	[[nodiscard]] inline constexpr scalar_type& y() noexcept
	{
		return i.y();
	}
	[[nodiscard]] inline constexpr const scalar_type& y() const noexcept
	{
		return i.y();
	}
	/// @}
	
	/// Returns a reference to the third element of the quaternion imaginary part.
	/// @{
	[[nodiscard]] inline constexpr scalar_type& z() noexcept
	{
		return i.z();
	}
	[[nodiscard]] inline constexpr const scalar_type& z() const noexcept
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
	[[nodiscard]] static quaternion rotate_x(scalar_type angle)
	{
		return {std::cos(angle * T{0.5}), std::sin(angle * T{0.5}), T{0}, T{0}};
	}
	
	/**
	 * Returns a quaternion representing a rotation about the y-axis.
	 *
	 * @param angle Angle of rotation, in radians.
	 *
	 * @return Quaternion representing an y-axis rotation.
	 */
	[[nodiscard]] static quaternion rotate_y(scalar_type angle)
	{
		return {std::cos(angle * T{0.5}), T{0}, std::sin(angle * T{0.5}), T{0}};
	}
	
	/**
	 * Returns a quaternion representing a rotation about the z-axis.
	 *
	 * @param angle Angle of rotation, in radians.
	 * @return Quaternion representing an z-axis rotation.
	 */
	[[nodiscard]] static quaternion rotate_z(scalar_type angle)
	{
		return {std::cos(angle * T{0.5}), T{0}, T{0}, std::sin(angle * T{0.5})};
	}
	
	/**
	 * Type-casts the quaternion scalars using `static_cast`.
	 *
	 * @tparam U Target scalar type.
	 *
	 * @return Type-casted quaternion.
	 */
	template <class U>
	[[nodiscard]] inline constexpr explicit operator quaternion<U>() const noexcept
	{
		return {static_cast<U>(r), vec3<U>(i)};
	}
	
	/**
	 * Constructs a matrix representing the rotation described by the quaternion.
	 *
	 * @return Rotation matrix.
	 */
	/// @{
	[[nodiscard]] constexpr explicit operator matrix_type() const noexcept
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
			T{1} - (yy + zz) * T{2}, (xy + zw) * T{2}, (xz - yw) * T{2},
			(xy - zw) * T{2}, T{1} - (xx + zz) * T{2}, (yz + xw) * T{2},
			(xz + yw) * T{2}, (yz - xw) * T{2}, T{1} - (xx + yy) * T{2}
		};
	}
	
	[[nodiscard]] inline constexpr matrix_type matrix() const noexcept
	{
		return matrix_type(*this);
	}
	/// @}
	
	/**
	 * Casts the quaternion to a 4-element vector, with the real part as the first element and the imaginary part as the following three elements.
	 *
	 * @return Vector containing the real and imaginary parts of the quaternion.
	 */
	[[nodiscard]] inline constexpr explicit operator vec4<T>() const noexcept
	{
		return {r, i.x(), i.y(), i.z()};
	}
	
	/// Returns a zero quaternion, where every scalar is equal to zero.
	[[nodiscard]] static constexpr quaternion zero() noexcept
	{
		return {};
	}
	
	/// Returns a rotation identity quaternion.
	[[nodiscard]] static constexpr quaternion identity() noexcept
	{
		return {T{1}, vector_type::zero()};
	}
};

/// Quaternion types.
namespace quaternion_types {

/// @copydoc math::quaternion
template <class T>
using quat = quaternion<T>;

/**
 * Quaternion with single-precision floating-point scalars.
 *
 * @tparam T Scalar type.
 */
using fquat = quat<float>;

/**
 * Quaternion with double-precision floating-point scalars.
 *
 * @tparam T Scalar type.
 */
using dquat = quat<double>;

} // namespace quaternion_types

// Bring quaternion types into math namespace
using namespace quaternion_types;

// Bring quaternion types into math::types namespace
namespace types { using namespace math::quaternion_types; }

/**
 * Adds two quaternions.
 *
 * @param a First quaternion.
 * @param b Second quaternion.
 *
 * @return Sum of the two quaternions.
 */
template <class T>
[[nodiscard]] constexpr quaternion<T> add(const quaternion<T>& a, const quaternion<T>& b) noexcept;

/**
 * Adds a quaternion and a scalar.
 *
 * @param a First value.
 * @param b Second second value.
 *
 * @return Sum of the quaternion and scalar.
 */
template <class T>
[[nodiscard]] constexpr quaternion<T> add(const quaternion<T>& a, T b) noexcept;

/**
 * Calculates the conjugate of a quaternion.
 *
 * @param q Quaternion from which the conjugate will be calculated.
 *
 * @return Conjugate of the quaternion.
 */
template <class T>
[[nodiscard]] constexpr quaternion<T> conjugate(const quaternion<T>& q) noexcept;

/**
 * Calculates the dot product of two quaternions.
 *
 * @param a First quaternion.
 * @param b Second quaternion.
 *
 * @return Dot product of the two quaternions.
 */
template <class T>
[[nodiscard]] constexpr T dot(const quaternion<T>& a, const quaternion<T>& b) noexcept;

/**
 * Divides a quaternion by another quaternion.
 *
 * @param a First value.
 * @param b Second value.
 *
 * @return Result of the division.
 */
template <class T>
[[nodiscard]] constexpr quaternion<T> div(const quaternion<T>& a, const quaternion<T>& b) noexcept;

/**
 * Divides a quaternion by a scalar.
 *
 * @param a Quaternion.
 * @param b Scalar.
 *
 * @return Result of the division.
 */
template <class T>
[[nodiscard]] constexpr quaternion<T> div(const quaternion<T>& a, T b) noexcept;

/**
 * Divides a scalar by a quaternion.
 *
 * @param a Scalar.
 * @param b Quaternion.
 *
 * @return Result of the division.
 */
template <class T>
[[nodiscard]] constexpr quaternion<T> div(T a, const quaternion<T>& b) noexcept;

/**
 * Calculates the inverse length of a quaternion.
 * 
 * @param q Quaternion to calculate the inverse length of.
 *
 * @return Inverse length of the quaternion.
 */
template <class T>
[[nodiscard]] T inv_length(const quaternion<T>& q);

/**
 * Calculates the length of a quaternion.
 * 
 * @param q Quaternion to calculate the length of.
 *
 * @return Length of the quaternion.
 */
template <class T>
[[nodiscard]] T length(const quaternion<T>& q);

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
[[nodiscard]] constexpr quaternion<T> lerp(const quaternion<T>& a, const quaternion<T>& b, T t) noexcept;

/**
 * Creates a unit quaternion rotation using forward and up vectors.
 *
 * @param forward Unit forward vector.
 * @param up Unit up vector.
 *
 * @return Unit rotation quaternion.
 */
template <class T>
[[nodiscard]] quaternion<T> look_rotation(const vec3<T>& forward, vec3<T> up);

/**
 * Multiplies two quaternions.
 *
 * @param a First quaternion.
 * @param b Second quaternion.
 *
 * @return Product of the two quaternions.
 */
template <class T>
[[nodiscard]] constexpr quaternion<T> mul(const quaternion<T>& a, const quaternion<T>& b) noexcept;

/**
 * Multiplies a quaternion by a scalar.
 *
 * @param a First value.
 * @param b Second value.
 *
 * @return Product of the quaternion and scalar.
 */
template <class T>
[[nodiscard]] constexpr quaternion<T> mul(const quaternion<T>& a, T b) noexcept;

/**
 * Rotates a vector by a unit quaternion.
 *
 * @param q Unit quaternion.
 * @param v Vector to rotate.
 *
 * @return Rotated vector.
 *
 * @warning @p q must be a unit quaternion.
 *
 * @see https://fgiesen.wordpress.com/2019/02/09/rotating-a-single-vector-using-a-quaternion/
 */
template <class T>
[[nodiscard]] constexpr vec3<T> mul(const quaternion<T>& q, const vec3<T>& v) noexcept;

/**
 * Rotates a vector by the inverse of a unit quaternion.
 *
 * @param v Vector to rotate.
 * @param q Unit quaternion.
 *
 * @return Rotated vector.
 *
 * @warning @p q must be a unit quaternion.
 */
template <class T>
[[nodiscard]] constexpr vec3<T> mul(const vec3<T>& v, const quaternion<T>& q) noexcept;

/**
 * Negates a quaternion.
 *
 * @param q Quaternion to negate.
 *
 * @return Negated quaternion.
 */
template <class T>
[[nodiscard]] constexpr quaternion<T> negate(const quaternion<T>& q) noexcept;

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
[[nodiscard]] quaternion<T> nlerp(const quaternion<T>& a, const quaternion<T>& b, T t);

/**
 * Normalizes a quaternion.
 *
 * @param q Quaternion to normalize.
 *
 * @return Unit quaternion.
 */
template <class T>
[[nodiscard]] quaternion<T> normalize(const quaternion<T>& q);

/**
 * Creates a rotation from an angle and axis.
 *
 * @param angle Angle of rotation (in radians).
 * @param axis Axis of rotation
 *
 * @return Quaternion representing the rotation.
 */
template <class T>
[[nodiscard]] quaternion<T> angle_axis(T angle, const vec3<T>& axis);

/**
 * Constructs a quaternion representing the minimum rotation from one direction to another direction.
 *
 * @param from Unit vector pointing in the source direction.
 * @param to Unit vector pointing in the target direction.
 * @param tolerance Floating-point tolerance.
 *
 * @return Unit quaternion representing the minimum rotation from direction @p from to direction @p to.
 *
 * @warning @p from and @p to must be unit vectors.
 */
template <class T>
[[nodiscard]] quaternion<T> rotation(const vec3<T>& from, const vec3<T>& to, T tolerance = T{1e-6});

/**
 * Constructs a quaternion representing an angle-limited rotation from one direction towards another direction.
 *
 * @param from Unit vector pointing in the source direction.
 * @param to Unit vector pointing in the target direction.
 * @param max_angle Maximum angle of rotation, in radians.
 *
 * @return Unit quaternion representing a rotation from direction @p from towards direction @p to.
 *
 * @warning @p from and @p to must be unit vectors.
 */
template <class T>
[[nodiscard]] quaternion<T> rotate_towards(const vec3<T>& from, const vec3<T>& to, T max_angle);

/**
 * Performs spherical linear interpolation between two quaternions.
 *
 * @param a First quaternion.
 * @param b Second quaternion.
 * @param t Interpolation factor.
 * @param tolerance Floating-point tolerance.
 *
 * @return Interpolated quaternion.
 */
template <class T>
[[nodiscard]] quaternion<T> slerp(const quaternion<T>& a, const quaternion<T>& b, T t, T tolerance = T{1e-6});

/**
 * Calculates the square length of a quaternion. The square length can be calculated faster than the length because a call to `std::sqrt` is saved.
 * 
 * @param q Quaternion to calculate the square length of.
 *
 * @return Square length of the quaternion.
 */
template <class T>
[[nodiscard]] constexpr T sqr_length(const quaternion<T>& q) noexcept;

/**
 * Subtracts a quaternion from another quaternion.
 *
 * @param a First quaternion.
 * @param b Second quaternion.
 *
 * @return Difference between the quaternions.
 */
template <class T>
[[nodiscard]] constexpr quaternion<T> sub(const quaternion<T>& a, const quaternion<T>& b) noexcept;

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
[[nodiscard]] constexpr quaternion<T> sub(const quaternion<T>& a, T b) noexcept;
template <class T>
[[nodiscard]] constexpr quaternion<T> sub(T a, const quaternion<T>& b) noexcept;
/// @}

/**
 * Decomposes a quaternion into swing and twist rotation components.
 *
 * @param[in] q Unit quaternion to decompose.
 * @param[in] twist_axis Axis of twist rotation.
 * @param[out] swing Swing rotation component.
 * @param[out] twist Twist rotation component.
 * @param[in] tolerance Floating-point tolerance.
 *
 * @return Array containing the swing rotation component, followed by the twist rotation component.
 *
 * @warning @p q must be a unit quaternion.
 * @warning @p twist_axis must be a unit vector.
 *
 * @see https://www.euclideanspace.com/maths/geometry/rotations/for/decomposition/
 */
template <class T>
[[nodiscard]] std::array<quaternion<T>, 2> swing_twist(const quaternion<T>& q, const vec3<T>& twist_axis, T tolerance = T{1e-6});

/**
 * Converts a 3x3 rotation matrix to a quaternion.
 *
 * @param m Rotation matrix.
 *
 * @return Unit quaternion representing the rotation described by @p m.
 */
template <class T>
[[nodiscard]] quaternion<T> quaternion_cast(const mat3<T>& m);

template <class T>
inline constexpr quaternion<T> add(const quaternion<T>& a, const quaternion<T>& b) noexcept
{
	return {a.r + b.r, a.i + b.i};
}

template <class T>
inline constexpr quaternion<T> add(const quaternion<T>& a, T b) noexcept
{
	return {a.r + b, a.i + b};
}

template <class T>
inline constexpr quaternion<T> conjugate(const quaternion<T>& q) noexcept
{
	return {q.r, -q.i};
}

template <class T>
inline constexpr T dot(const quaternion<T>& a, const quaternion<T>& b) noexcept
{
	return a.r * b.r + dot(a.i, b.i);
}

template <class T>
inline constexpr quaternion<T> div(const quaternion<T>& a, const quaternion<T>& b) noexcept
{
	return {a.r / b.r, a.i / b.i};
}

template <class T>
inline constexpr quaternion<T> div(const quaternion<T>& a, T b) noexcept
{
	return {a.r / b, a.i / b};
}

template <class T>
inline constexpr quaternion<T> div(T a, const quaternion<T>& b) noexcept
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
inline constexpr quaternion<T> lerp(const quaternion<T>& a, const quaternion<T>& b, T t) noexcept
{
	return
	{
		(b.r - a.r) * t + a.r,
		(b.i - a.i) * t + a.i
	};
}

template <class T>
quaternion<T> look_rotation(const vec3<T>& forward, vec3<T> up)
{
	const vec3<T> right = normalize(cross(forward, up));
	up = cross(right, forward);
	
	const mat3<T> m =
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
		a.w() * b.w() - a.x() * b.x() - a.y() * b.y() - a.z() * b.z(),
		a.w() * b.x() + a.x() * b.w() + a.y() * b.z() - a.z() * b.y(),  
		a.w() * b.y() - a.x() * b.z() + a.y() * b.w() + a.z() * b.x(),
		a.w() * b.z() + a.x() * b.y() - a.y() * b.x() + a.z() * b.w()
	};
}

template <class T>
inline constexpr quaternion<T> mul(const quaternion<T>& a, T b) noexcept
{
	return {a.r * b, a.i * b};
}

template <class T>
constexpr vec3<T> mul(const quaternion<T>& q, const vec3<T>& v) noexcept
{
	const auto t = cross(q.i, v) * T{2};
	return v + q.r * t + cross(q.i, t);
}

template <class T>
inline constexpr vec3<T> mul(const vec3<T>& v, const quaternion<T>& q) noexcept
{
	return mul(conjugate(q), v);
}

template <class T>
inline constexpr quaternion<T> negate(const quaternion<T>& q) noexcept
{
	return {-q.r, -q.i};
}

template <class T>
quaternion<T> nlerp(const quaternion<T>& a, const quaternion<T>& b, T t)
{
	return normalize(add(mul(a, T{1} - t), mul(b, std::copysign(t, dot(a, b)))));
}

template <class T>
inline quaternion<T> normalize(const quaternion<T>& q)
{
	return mul(q, inv_length(q));
}

template <class T>
quaternion<T> angle_axis(T angle, const vec3<T>& axis)
{
	return {std::cos(angle * T{0.5}), axis * std::sin(angle * T{0.5})};
}

template <class T>
quaternion<T> rotation(const vec3<T>& from, const vec3<T>& to, T tolerance)
{
	const auto cos_theta = dot(from, to);
	
	if (cos_theta <= T{-1} + tolerance)
	{
		// Direction vectors are opposing, return 180 degree rotation about arbitrary axis
		return quaternion<T>{T{0}, {T{1}, T{0}, T{0}}};
	}
	else if (cos_theta >= T{1} - tolerance)
	{
		// Direction vectors are parallel, return identity quaternion
		return quaternion<T>::identity();
	}
	else
	{
		const auto r = cos_theta + T{1};
		const auto i = cross(from, to);
		const auto inv_length = T{1.0} / std::sqrt(r + r);
		
		return quaternion<T>{r * inv_length, i * inv_length};
	}
}

template <class T>
quaternion<T> rotate_towards(const vec3<T>& from, const vec3<T>& to, T max_angle)
{
	const auto angle = std::acos(dot(from, to));
	const auto axis = cross(from, to);
	return angle_axis(std::min(max_angle, angle), axis);
}

template <class T>
quaternion<T> slerp(const quaternion<T>& a, const quaternion<T>& b, T t, T tolerance)
{
	T cos_theta = dot(a, b);
	if (cos_theta >= T{1} - tolerance)
	{
		// Use linear interpolation if quaternions are nearly aligned
		return normalize(lerp(a, b, t));
	}
	
	// Clamp to acos domain
	cos_theta = std::min<T>(std::max<T>(cos_theta, T{-1}), T{1});
	
	const T theta = std::acos(cos_theta) * t;
	
	const quaternion<T> c = normalize(sub(b, mul(a, cos_theta)));
	
	return add(mul(a, std::cos(theta)), mul(c, std::sin(theta)));
}

template <class T>
inline constexpr T sqr_length(const quaternion<T>& q) noexcept
{
	return q.r * q.r + sqr_length(q.i);
}

template <class T>
inline constexpr quaternion<T> sub(const quaternion<T>& a, const quaternion<T>& b) noexcept
{
	return {a.r - b.r, a.i - b.i};
}

template <class T>
inline constexpr quaternion<T> sub(const quaternion<T>& a, T b) noexcept
{
	return {a.r - b, a.i - b};
}

template <class T>
inline constexpr quaternion<T> sub(T a, const quaternion<T>& b) noexcept
{
	return {a - b.r, a - b.i};
}

template <class T>
std::array<quaternion<T>, 2> swing_twist(const quaternion<T>& q, const vec3<T>& twist_axis, T tolerance)
{
	quaternion<T> swing;
	quaternion<T> twist;
	
	if (sqr_length(q.i) <= tolerance)
	{
		// Singularity, rotate 180 degrees about twist axis
		twist = angle_axis(pi<T>, twist_axis);
		
		const auto rotated_twist_axis = mul(q, twist_axis);
		const auto swing_axis = cross(twist_axis, rotated_twist_axis);
		const auto swing_axis_sqr_length = sqr_length(swing_axis);
		
		if (swing_axis_sqr_length <= tolerance)
		{
			// Swing axis and twist axis are parallel, no swing
			swing = quaternion<T>::identity();
		}
		else
		{
			const auto cos_swing_angle = std::min<T>(std::max<T>(dot(twist_axis, rotated_twist_axis), T{-1}), T{1});
			swing = angle_axis(std::acos(cos_swing_angle), swing_axis * (T{1} / std::sqrt(swing_axis_sqr_length)));
		}
	}
	else
	{
		twist = normalize(quaternion<T>{q.r, twist_axis * dot(twist_axis, q.i)});
		swing = mul(q, conjugate(twist));
	}
	
	return {std::move(swing), std::move(twist)};
}

template <class T>
quaternion<T> quaternion_cast(const mat3<T>& m)
{
	const T t = trace(m);
	
	if (t > T{0})
	{
		const T s = T{0.5} / std::sqrt(t + T{1});
		return
		{
			T{0.25} / s,
			(m[1][2] - m[2][1]) * s,
			(m[2][0] - m[0][2]) * s,
			(m[0][1] - m[1][0]) * s
		};
	}
	else
	{
		if (m[0][0] > m[1][1] && m[0][0] > m[2][2])
		{
			const T s = T{2} * std::sqrt(T{1} + m[0][0] - m[1][1] - m[2][2]);
			
			return
			{
				(m[1][2] - m[2][1]) / s,
				T{0.25} * s,
				(m[1][0] + m[0][1]) / s,
				(m[2][0] + m[0][2]) / s
			};
		}
		else if (m[1][1] > m[2][2])
		{
			const T s = T{2} * std::sqrt(T{1} + m[1][1] - m[0][0] - m[2][2]);
			return
			{
				(m[2][0] - m[0][2]) / s,
				(m[1][0] + m[0][1]) / s,
				T{0.25} * s,
				(m[2][1] + m[1][2]) / s
			};
		}
		else
		{
			const T s = T{2} * std::sqrt(T{1} + m[2][2] - m[0][0] - m[1][1]);
			return
			{
				(m[0][1] - m[1][0]) / s,
				(m[2][0] + m[0][2]) / s,
				(m[2][1] + m[1][2]) / s,
				T{0.25} * s
			};
		}
	}
}

namespace operators {

/// @copydoc add(const quaternion<T>&, const quaternion<T>&)
template <class T>
inline constexpr quaternion<T> operator+(const quaternion<T>& a, const quaternion<T>& b) noexcept
{
	return add(a, b);
}

/// @copydoc add(const quaternion<T>&, T)
/// @{
template <class T>
inline constexpr quaternion<T> operator+(const quaternion<T>& a, T b) noexcept
{
	return add(a, b);
}
template <class T>
inline constexpr quaternion<T> operator+(T a, const quaternion<T>& b) noexcept
{
	return add(b, a);
}
/// @}

/// @copydoc div(const quaternion<T>&, const quaternion<T>&)
template <class T>
inline constexpr quaternion<T> operator/(const quaternion<T>& a, const quaternion<T>& b) noexcept
{
	return div(a, b);
}

/// @copydoc div(const quaternion<T>&, T)
template <class T>
inline constexpr quaternion<T> operator/(const quaternion<T>& a, T b) noexcept
{
	return div(a, b);
}

/// @copydoc div(T, const quaternion<T>&)
template <class T>
inline constexpr quaternion<T> operator/(T a, const quaternion<T>& b) noexcept
{
	return div(a, b);
}

/// @copydoc mul(const quaternion<T>&, const quaternion<T>&)
template <class T>
inline constexpr quaternion<T> operator*(const quaternion<T>& a, const quaternion<T>& b) noexcept
{
	return mul(a, b);
}

/// @copydoc mul(const quaternion<T>&, T)
/// @{
template <class T>
inline constexpr quaternion<T> operator*(const quaternion<T>& a, T b) noexcept
{
	return mul(a, b);
}
template <class T>
inline constexpr quaternion<T> operator*(T a, const quaternion<T>& b) noexcept
{
	return mul(b, a);
}
/// @}

/// @copydoc mul(const quaternion<T>&, const vec3<T>&)
template <class T>
inline constexpr vec3<T> operator*(const quaternion<T>& q, const vec3<T>& v) noexcept
{
	return mul(q, v);
}

/// @copydoc mul(const vec3<T>&, const quaternion<T>&)
template <class T>
inline constexpr vec3<T> operator*(const vec3<T>& v, const quaternion<T>& q) noexcept
{
	return mul(v, q);
}

/// @copydoc sub(const quaternion<T>&, const quaternion<T>&)
template <class T>
inline constexpr quaternion<T> operator-(const quaternion<T>& a, const quaternion<T>& b) noexcept
{
	return sub(a, b);
}

/// @copydoc sub(const quaternion<T>&, T)
/// @{
template <class T>
inline constexpr quaternion<T> operator-(const quaternion<T>& a, T b) noexcept
{
	return sub(a, b);
}
template <class T>
inline constexpr quaternion<T> operator-(T a, const quaternion<T>& b) noexcept
{
	return sub(a, b);
}
/// @}

/// @copydoc negate(const quaternion<T>&)
template <class T>
inline constexpr quaternion<T> operator-(const quaternion<T>& q) noexcept
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
inline constexpr quaternion<T>& operator+=(quaternion<T>& a, const quaternion<T>& b) noexcept
{
	return (a = a + b);
}
template <class T>
inline constexpr quaternion<T>& operator+=(quaternion<T>& a, T b) noexcept
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
inline constexpr quaternion<T>& operator-=(quaternion<T>& a, const quaternion<T>& b) noexcept
{
	return (a = a - b);
}
template <class T>
inline constexpr quaternion<T>& operator-=(quaternion<T>& a, T b) noexcept
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
inline constexpr quaternion<T>& operator*=(quaternion<T>& a, const quaternion<T>& b) noexcept
{
	return (a = a * b);
}
template <class T>
inline constexpr quaternion<T>& operator*=(quaternion<T>& a, T b) noexcept
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
inline constexpr quaternion<T>& operator/=(quaternion<T>& a, const quaternion<T>& b) noexcept
{
	return (a = a / b);
}
template <class T>
inline constexpr quaternion<T>& operator/=(quaternion<T>& a, T b) noexcept
{
	return (a = a / b);
}
/// @}

} // namespace operators

} // namespace math

// Bring quaternion operators into global namespace
using namespace math::operators;

#endif // ANTKEEPER_MATH_QUATERNION_HPP

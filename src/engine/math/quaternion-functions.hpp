// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_MATH_QUATERNION_FUNCTIONS_HPP
#define ANTKEEPER_MATH_QUATERNION_FUNCTIONS_HPP

#include <engine/math/quaternion-types.hpp>
#include <engine/math/quaternion-constants.hpp>
#include <engine/math/constants.hpp>
#include <engine/math/vector.hpp>
#include <engine/math/matrix.hpp>
#include <array>
#include <cmath>
#include <utility>

// export module math.quaternion:functions;
// import :type;
// import :constants;
// import math.constants;
// import math.vector;
// import math.matrix;
// import <array>;
// import <cmath>;
// import <utility>;

namespace math {
namespace functions {

/// Adds two quaternions.
/// @param a First quaternion.
/// @param b Second quaternion.
/// @return Sum of the two quaternions.
template <class T>
[[nodiscard]] constexpr quaternion<T> add(const quaternion<T>& a, const quaternion<T>& b) noexcept;

/// Adds a quaternion and a scalar.
/// @param a First value.
/// @param b Second second value.
/// @return Sum of the quaternion and scalar.
template <class T>
[[nodiscard]] constexpr quaternion<T> add(const quaternion<T>& a, T b) noexcept;

/// Calculates the conjugate of a quaternion.
/// @param q Quaternion from which the conjugate will be calculated.
/// @return Conjugate of the quaternion.
template <class T>
[[nodiscard]] constexpr quaternion<T> conjugate(const quaternion<T>& q) noexcept;

/// Calculates the dot product of two quaternions.
/// @param a First quaternion.
/// @param b Second quaternion.
/// @return Dot product of the two quaternions.
template <class T>
[[nodiscard]] constexpr T dot(const quaternion<T>& a, const quaternion<T>& b) noexcept;

/// Divides a quaternion by another quaternion.
/// @param a First value.
/// @param b Second value.
/// @return Result of the division.
template <class T>
[[nodiscard]] constexpr quaternion<T> div(const quaternion<T>& a, const quaternion<T>& b) noexcept;

/// Divides a quaternion by a scalar.
/// @param a Quaternion.
/// @param b Scalar.
/// @return Result of the division.
template <class T>
[[nodiscard]] constexpr quaternion<T> div(const quaternion<T>& a, T b) noexcept;

/// Divides a scalar by a quaternion.
/// @param a Scalar.
/// @param b Quaternion.
/// @return Result of the division.
template <class T>
[[nodiscard]] constexpr quaternion<T> div(T a, const quaternion<T>& b) noexcept;

/// Calculates the inverse length of a quaternion.
/// @param q Quaternion to calculate the inverse length of.
/// @return Inverse length of the quaternion.
template <class T>
[[nodiscard]] T inv_length(const quaternion<T>& q);

/// Calculates the length of a quaternion.
/// @param q Quaternion to calculate the length of.
/// @return Length of the quaternion.
template <class T>
[[nodiscard]] T length(const quaternion<T>& q);

/// Performs linear interpolation between two quaternions.
/// @param a First quaternion.
/// @param b Second quaternion.
/// @param t Interpolation factor.
/// @return Interpolated quaternion.
template <class T>
[[nodiscard]] constexpr quaternion<T> lerp(const quaternion<T>& a, const quaternion<T>& b, T t) noexcept;

/// Creates a unit quaternion rotation using forward and up vectors.
/// @param forward Unit forward vector.
/// @param up Unit up vector.
/// @return Unit rotation quaternion.
template <class T>
[[nodiscard]] quaternion<T> look_rotation(const vec3<T>& forward, vec3<T> up);

/// Multiplies two quaternions.
/// @param a First quaternion.
/// @param b Second quaternion.
/// @return Product of the two quaternions.
template <class T>
[[nodiscard]] constexpr quaternion<T> mul(const quaternion<T>& a, const quaternion<T>& b) noexcept;

/// Multiplies a quaternion by a scalar.
/// @param a First value.
/// @param b Second value.
/// @return Product of the quaternion and scalar.
template <class T>
[[nodiscard]] constexpr quaternion<T> mul(const quaternion<T>& a, T b) noexcept;

/// Rotates a vector by a unit quaternion.
/// @param q Unit quaternion.
/// @param v Vector to rotate.
/// @return Rotated vector.
/// @warning @p q must be a unit quaternion.
/// @see https://fgiesen.wordpress.com/2019/02/09/rotating-a-single-vector-using-a-quaternion/
template <class T>
[[nodiscard]] constexpr vec3<T> mul(const quaternion<T>& q, const vec3<T>& v) noexcept;

/// Rotates a vector by the inverse of a unit quaternion.
/// @param v Vector to rotate.
/// @param q Unit quaternion.
/// @return Rotated vector.
/// @warning @p q must be a unit quaternion.
template <class T>
[[nodiscard]] constexpr vec3<T> mul(const vec3<T>& v, const quaternion<T>& q) noexcept;

/// Negates a quaternion.
/// @param q Quaternion to negate.
/// @return Negated quaternion.
template <class T>
[[nodiscard]] constexpr quaternion<T> negate(const quaternion<T>& q) noexcept;

/// Performs normalized linear interpolation between two quaternions.
/// @param a First quaternion.
/// @param b Second quaternion.
/// @param t Interpolation factor.
/// @return Interpolated quaternion.
template <class T>
[[nodiscard]] quaternion<T> nlerp(const quaternion<T>& a, const quaternion<T>& b, T t);

/// Normalizes a quaternion.
/// @param q Quaternion to normalize.
/// @return Unit quaternion.
template <class T>
[[nodiscard]] quaternion<T> normalize(const quaternion<T>& q);

/// Creates a rotation from an angle and axis.
/// @param angle Angle of rotation (in radians).
/// @param axis Axis of rotation
/// @return Quaternion representing the rotation.
template <class T>
[[nodiscard]] quaternion<T> angle_axis(T angle, const vec3<T>& axis);

/// Constructs a quaternion representing the minimum rotation from one direction to another direction.
/// @param from Unit vector pointing in the source direction.
/// @param to Unit vector pointing in the target direction.
/// @param tolerance Floating-point tolerance.
/// @return Unit quaternion representing the minimum rotation from direction @p from to direction @p to.
/// @warning @p from and @p to must be unit vectors.
template <class T>
[[nodiscard]] quaternion<T> rotation(const vec3<T>& from, const vec3<T>& to, T tolerance = T{1e-6});

/// Constructs a quaternion representing an angle-limited rotation from one direction towards another direction.
/// @param from Unit vector pointing in the source direction.
/// @param to Unit vector pointing in the target direction.
/// @param max_angle Maximum angle of rotation, in radians.
/// @return Unit quaternion representing a rotation from direction @p from towards direction @p to.
/// @warning @p from and @p to must be unit vectors.
template <class T>
[[nodiscard]] quaternion<T> rotate_towards(const vec3<T>& from, const vec3<T>& to, T max_angle);

/// Performs spherical linear interpolation between two quaternions.
/// @param a First quaternion.
/// @param b Second quaternion.
/// @param t Interpolation factor.
/// @param tolerance Floating-point tolerance.
/// @return Interpolated quaternion.
template <class T>
[[nodiscard]] quaternion<T> slerp(const quaternion<T>& a, const quaternion<T>& b, T t, T tolerance = T{1e-6});

/// Calculates the square length of a quaternion. The square length can be calculated faster than the length because a call to `std::sqrt` is saved.
/// @param q Quaternion to calculate the square length of.
/// @return Square length of the quaternion.
template <class T>
[[nodiscard]] constexpr T sqr_length(const quaternion<T>& q) noexcept;

/// Subtracts a quaternion from another quaternion.
/// @param a First quaternion.
/// @param b Second quaternion.
/// @return Difference between the quaternions.
template <class T>
[[nodiscard]] constexpr quaternion<T> sub(const quaternion<T>& a, const quaternion<T>& b) noexcept;

/// Subtracts a quaternion and a scalar.
/// @param a First value.
/// @param b Second second.
/// @return Difference between the quaternion and scalar.
template <class T>
[[nodiscard]] constexpr quaternion<T> sub(const quaternion<T>& a, T b) noexcept;

/// Subtracts a quaternion and a scalar.
/// @param a First value.
/// @param b Second second.
/// @return Difference between the quaternion and scalar.
template <class T>
[[nodiscard]] constexpr quaternion<T> sub(T a, const quaternion<T>& b) noexcept;

/// Decomposes a quaternion into swing and twist rotation components.
/// @param[in] q Unit quaternion to decompose.
/// @param[in] twist_axis Axis of twist rotation.
/// @param[in] tolerance Floating-point tolerance.
/// @return Array containing the swing rotation component, followed by the twist rotation component.
/// @warning @p q must be a unit quaternion.
/// @warning @p twist_axis must be a unit vector.
/// @see https://www.euclideanspace.com/maths/geometry/rotations/for/decomposition/
template <class T>
[[nodiscard]] std::array<quaternion<T>, 2> swing_twist(const quaternion<T>& q, const vec3<T>& twist_axis, T tolerance = T{1e-6});

/// Converts a 3x3 rotation matrix to a quaternion.
/// @param m Rotation matrix.
/// @return Unit quaternion representing the rotation described by @p m.
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
	const auto t = cross(v, q.i) * T{2};
	return v + q.r * t + cross(t, q.i);
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
		return identity<quaternion<T>>;
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
			swing = identity<quaternion<T>>;
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

} // namespace functions

// Bring math::functions into math namespace
using namespace functions;

} // namespace math

/// Adds two quaternions.
/// @param a First quaternion.
/// @param b Second quaternion.
/// @return Sum of the two quaternions.
template <class T>
[[nodiscard]] inline constexpr math::quaternion<T> operator+(const math::quaternion<T>& a, const math::quaternion<T>& b) noexcept
{
	return math::add(a, b);
}

/// Adds a quaternion and a scalar.
/// @param a First value.
/// @param b Second second value.
/// @return Sum of the quaternion and scalar.
template <class T>
[[nodiscard]] inline constexpr math::quaternion<T> operator+(const math::quaternion<T>& a, T b) noexcept
{
	return math::add(a, b);
}

/// Adds a quaternion and a scalar.
/// @param a First value.
/// @param b Second second value.
/// @return Sum of the quaternion and scalar.
template <class T>
[[nodiscard]] inline constexpr math::quaternion<T> operator+(T a, const math::quaternion<T>& b) noexcept
{
	return math::add(b, a);
}

/// Divides a quaternion by another quaternion.
/// @param a First value.
/// @param b Second value.
/// @return Result of the division.
template <class T>
[[nodiscard]] inline constexpr math::quaternion<T> operator/(const math::quaternion<T>& a, const math::quaternion<T>& b) noexcept
{
	return math::div(a, b);
}

/// Divides a quaternion by a scalar.
/// @param a Quaternion.
/// @param b Scalar.
/// @return Result of the division.
template <class T>
[[nodiscard]] inline constexpr math::quaternion<T> operator/(const math::quaternion<T>& a, T b) noexcept
{
	return math::div(a, b);
}

/// Divides a scalar by a quaternion.
/// @param a Scalar.
/// @param b Quaternion.
/// @return Result of the division.
template <class T>
[[nodiscard]] inline constexpr math::quaternion<T> operator/(T a, const math::quaternion<T>& b) noexcept
{
	return math::div(a, b);
}

/// Multiplies two quaternions.
/// @param a First quaternion.
/// @param b Second quaternion.
/// @return Product of the two quaternions.
template <class T>
[[nodiscard]] inline constexpr math::quaternion<T> operator*(const math::quaternion<T>& a, const math::quaternion<T>& b) noexcept
{
	return math::mul(a, b);
}

/// Multiplies a quaternion by a scalar.
/// @param a First value.
/// @param b Second value.
/// @return Product of the quaternion and scalar.
template <class T>
[[nodiscard]] inline constexpr math::quaternion<T> operator*(const math::quaternion<T>& a, T b) noexcept
{
	return math::mul(a, b);
}

/// Multiplies a quaternion by a scalar.
/// @param a First value.
/// @param b Second value.
/// @return Product of the quaternion and scalar.
template <class T>
[[nodiscard]] inline constexpr math::quaternion<T> operator*(T a, const math::quaternion<T>& b) noexcept
{
	return math::mul(b, a);
}

/// Rotates a vector by a unit quaternion.
/// @param q Unit quaternion.
/// @param v Vector to rotate.
/// @return Rotated vector.
/// @warning @p q must be a unit quaternion.
template <class T>
[[nodiscard]] inline constexpr math::vec3<T> operator*(const math::quaternion<T>& q, const math::vec3<T>& v) noexcept
{
	return math::mul(q, v);
}

/// Rotates a vector by the inverse of a unit quaternion.
/// @param v Vector to rotate.
/// @param q Unit quaternion.
/// @return Rotated vector.
/// @warning @p q must be a unit quaternion.
template <class T>
[[nodiscard]] inline constexpr math::vec3<T> operator*(const math::vec3<T>& v, const math::quaternion<T>& q) noexcept
{
	return math::mul(v, q);
}

/// Subtracts a quaternion from another quaternion.
/// @param a First quaternion.
/// @param b Second quaternion.
/// @return Difference between the quaternions.
template <class T>
[[nodiscard]] inline constexpr math::quaternion<T> operator-(const math::quaternion<T>& a, const math::quaternion<T>& b) noexcept
{
	return math::sub(a, b);
}

/// Subtracts a quaternion and a scalar.
/// @param a First value.
/// @param b Second second.
/// @return Difference between the quaternion and scalar.
template <class T>
[[nodiscard]] inline constexpr math::quaternion<T> operator-(const math::quaternion<T>& a, T b) noexcept
{
	return math::sub(a, b);
}

/// Subtracts a quaternion and a scalar.
/// @param a First value.
/// @param b Second second.
/// @return Difference between the quaternion and scalar.
template <class T>
[[nodiscard]] inline constexpr math::quaternion<T> operator-(T a, const math::quaternion<T>& b) noexcept
{
	return math::sub(a, b);
}

/// Negates a quaternion.
/// @param q Quaternion to negate.
/// @return Negated quaternion.
template <class T>
[[nodiscard]] inline constexpr math::quaternion<T> operator-(const math::quaternion<T>& q) noexcept
{
	return math::negate(q);
}

/// Adds two values and stores the result in the first value.
/// @param a First value.
/// @param b Second value.
/// @return Reference to the first value.
template <class T>
inline constexpr math::quaternion<T>& operator+=(math::quaternion<T>& a, const math::quaternion<T>& b) noexcept
{
	return (a = a + b);
}

/// Adds two values and stores the result in the first value.
/// @param a First value.
/// @param b Second value.
/// @return Reference to the first value.
template <class T>
inline constexpr math::quaternion<T>& operator+=(math::quaternion<T>& a, T b) noexcept
{
	return (a = a + b);
}

/// Subtracts the first value by the second value and stores the result in the first value.
/// @param a First value.
/// @param b Second value.
/// @return Reference to the first value.
template <class T>
inline constexpr math::quaternion<T>& operator-=(math::quaternion<T>& a, const math::quaternion<T>& b) noexcept
{
	return (a = a - b);
}

/// Subtracts the first value by the second value and stores the result in the first value.
/// @param a First value.
/// @param b Second value.
/// @return Reference to the first value.
template <class T>
inline constexpr math::quaternion<T>& operator-=(math::quaternion<T>& a, T b) noexcept
{
	return (a = a - b);
}

/// Multiplies two values and stores the result in the first value.
/// @param a First value.
/// @param b Second value.
/// @return Reference to the first value.
template <class T>
inline constexpr math::quaternion<T>& operator*=(math::quaternion<T>& a, const math::quaternion<T>& b) noexcept
{
	return (a = a * b);
}

/// Multiplies two values and stores the result in the first value.
/// @param a First value.
/// @param b Second value.
/// @return Reference to the first value.
template <class T>
inline constexpr math::quaternion<T>& operator*=(math::quaternion<T>& a, T b) noexcept
{
	return (a = a * b);
}

/// Divides the first value by the second value and stores the result in the first value.
/// @param a First value.
/// @param b Second value.
/// @return Reference to the first value.
template <class T>
inline constexpr math::quaternion<T>& operator/=(math::quaternion<T>& a, const math::quaternion<T>& b) noexcept
{
	return (a = a / b);
}

/// Divides the first value by the second value and stores the result in the first value.
/// @param a First value.
/// @param b Second value.
/// @return Reference to the first value.
template <class T>
inline constexpr math::quaternion<T>& operator/=(math::quaternion<T>& a, T b) noexcept
{
	return (a = a / b);
}

#endif // ANTKEEPER_MATH_QUATERNION_FUNCTIONS_HPP

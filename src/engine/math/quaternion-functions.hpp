// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <engine/math/quaternion-type.hpp>
#include <engine/math/quaternion-constants.hpp>
#include <engine/math/constants.hpp>
#include <engine/math/functions.hpp>
#include <engine/math/vector.hpp>
#include <engine/math/matrix.hpp>
#include <engine/math/axis-angle.hpp>
#include <engine/math/basis.hpp>
#include <array>
#include <utility>

namespace engine::math::inline types
{
	/// @name Quaternion operators
	/// @{

	/// Adds two quaternions.
	/// @param lhs Quaternion on the left-hand side.
	/// @param rhs Quaternion on the right-hand side.
	/// @return Sum of the two quaternions.
	/// @relates quaternion
	template <class T>
	[[nodiscard]] inline constexpr quat<T> operator+(const quat<T>& lhs, const quat<T>& rhs) noexcept
	{
		return {lhs.r + rhs.r, lhs.i + rhs.i};
	}

	/// Adds a quaternion and a scalar.
	/// @param lhs Quaternion on the left-hand side.
	/// @param rhs Scalar on the right-hand side.
	/// @return Sum of the quaternion and scalar.
	/// @relates quaternion
	template <class T>
	[[nodiscard]] inline constexpr quat<T> operator+(const quat<T>& lhs, T rhs) noexcept
	{
		return {lhs.r + rhs, lhs.i + rhs};
	}

	/// Adds a quaternion and a scalar.
	/// @param lhs Scalar on the left-hand side.
	/// @param rhs Quaternion on the right-hand side.
	/// @return Sum of the quaternion and scalar.
	/// @relates quaternion
	template <class T>
	[[nodiscard]] inline constexpr quat<T> operator+(T lhs, const quat<T>& rhs) noexcept
	{
		return rhs + lhs;
	}

	/// Returns a copy of the quaternion with negated elements.
	/// @param q Quaternion to negate.
	/// @return Copy of the quaternion with negated elements.
	/// @relates quaternion
	template <class T>
	[[nodiscard]] inline constexpr quat<T> operator-(const quat<T>& q) noexcept
	{
		return {-q.r, -q.i};
	}

	/// Subtracts a quaternion from another quaternion.
	/// @param lhs Quaternion on the left-hand side.
	/// @param rhs Quaternion on the right-hand side.
	/// @return Difference between the quaternions.
	/// @relates quaternion
	template <class T>
	[[nodiscard]] inline constexpr quat<T> operator-(const quat<T>& lhs, const quat<T>& rhs) noexcept
	{
		return {lhs.r - rhs.r, lhs.i - rhs.i};
	}

	/// Subtracts a quaternion and a scalar.
	/// @param lhs Quaternion on the left-hand side.
	/// @param rhs Scalar on the right-hand side.
	/// @return Difference between the quaternion and scalar.
	/// @relates quaternion
	template <class T>
	[[nodiscard]] inline constexpr quat<T> operator-(const quat<T>& lhs, T rhs) noexcept
	{
		return {lhs.r - rhs, lhs.i - rhs};
	}

	/// Subtracts a quaternion and a scalar.
	/// @param lhs Scalar on the left-hand side.
	/// @param rhs Quaternion on the right-hand side.
	/// @return Difference between the quaternion and scalar.
	/// @relates quaternion
	template <class T>
	[[nodiscard]] inline constexpr quat<T> operator-(T lhs, const quat<T>& rhs) noexcept
	{
		return {lhs - rhs.r, lhs - rhs.i};
	}

	/// Multiplies two quaternions.
	/// @param lhs Quaternion on the left-hand side.
	/// @param rhs Quaternion on the right-hand side.
	/// @return Product of the two quaternions.
	/// @relates quaternion
	template <class T>
	[[nodiscard]] inline constexpr quat<T> operator*(const quat<T>& lhs, const quat<T>& rhs) noexcept
	{
		return
		{
			lhs.w() * rhs.w() - lhs.x() * rhs.x() - lhs.y() * rhs.y() - lhs.z() * rhs.z(),
			lhs.w() * rhs.x() + lhs.x() * rhs.w() + lhs.y() * rhs.z() - lhs.z() * rhs.y(),
			lhs.w() * rhs.y() - lhs.x() * rhs.z() + lhs.y() * rhs.w() + lhs.z() * rhs.x(),
			lhs.w() * rhs.z() + lhs.x() * rhs.y() - lhs.y() * rhs.x() + lhs.z() * rhs.w()
		};
	}

	/// Multiplies a quaternion by a scalar.
	/// @param lhs Quaternion on the left-hand side.
	/// @param rhs Scalar on the right-hand side.
	/// @return Product of the quaternion and scalar.
	/// @relates quaternion
	template <class T>
	[[nodiscard]] inline constexpr quat<T> operator*(const quat<T>& lhs, T rhs) noexcept
	{
		return {lhs.r * rhs, lhs.i * rhs};
	}

	/// Multiplies a quaternion by a scalar.
	/// @param lhs Scalar on the left-hand side.
	/// @param rhs Quaternion on the right-hand side.
	/// @return Product of the quaternion and scalar.
	/// @relates quaternion
	template <class T>
	[[nodiscard]] inline constexpr quat<T> operator*(T lhs, const quat<T>& rhs) noexcept
	{
		return rhs * lhs;
	}

	/// Rotates a vector by a unit quaternion.
	/// @param lhs Unit quaternion on the left-hand side.
	/// @param rhs Vector to rotate on the right-hand side.
	/// @return Rotated vector.
	/// @warning @p lhs must be a unit quaternion.
	/// @see https://fgiesen.wordpress.com/2019/02/09/rotating-a-single-vector-using-a-quaternion/
	/// @relates quaternion
	template <class T>
	[[nodiscard]] inline constexpr vec3<T> operator*(const quat<T>& lhs, const vec3<T>& rhs) noexcept
	{
		const auto t = cross(lhs.i, rhs) * T{2};
		return rhs + lhs.r * t + cross(lhs.i, t);
	}

	/// Rotates a vector by the inverse of a unit quaternion.
	/// @param lhs Vector to rotate on the left-hand side.
	/// @param rhs Unit quaternion on the right-hand side.
	/// @return Rotated vector.
	/// @warning @p rhs must be a unit quaternion.
	/// @relates quaternion
	template <class T>
	[[nodiscard]] inline constexpr vec3<T> operator*(const vec3<T>& lhs, const quat<T>& rhs) noexcept
	{
		const auto t = cross(lhs, rhs.i) * T{2};
		return lhs + rhs.r * t + cross(t, rhs.i);
	}

	/// Divides a quaternion by another quaternion.
	/// @param lhs Quaternion on the left-hand side.
	/// @param rhs Quaternion on the right-hand side.
	/// @return Result of the division.
	/// @relates quaternion
	template <class T>
	[[nodiscard]] inline constexpr quat<T> operator/(const quat<T>& lhs, const quat<T>& rhs) noexcept
	{
		return {lhs.r / rhs.r, lhs.i / rhs.i};
	}

	/// Divides a quaternion by a scalar.
	/// @param lhs Quaternion on the left-hand side.
	/// @param rhs Scalar on the right-hand side.
	/// @return Result of the division.
	/// @relates quaternion
	template <class T>
	[[nodiscard]] inline constexpr quat<T> operator/(const quat<T>& lhs, T rhs) noexcept
	{
		return {lhs.r / rhs, lhs.i / rhs};
	}

	/// Divides a scalar by a quaternion.
	/// @param lhs Scalar on the left-hand side.
	/// @param rhs Quaternion on the right-hand side.
	/// @return Result of the division.
	/// @relates quaternion
	template <class T>
	[[nodiscard]] inline constexpr quat<T> operator/(T lhs, const quat<T>& rhs) noexcept
	{
		return {lhs / rhs.r, lhs / rhs.i};
	}

	/// Adds two values and stores the result in the first value.
	/// @param lhs Quaternion on the left-hand side.
	/// @param rhs Quaternion on the right-hand side.
	/// @return Reference to the quaternion on the left-hand side.
	/// @relates quaternion
	template <class T>
	inline constexpr quat<T>& operator+=(quat<T>& lhs, const quat<T>& rhs) noexcept
	{
		return lhs = lhs + rhs;
	}

	/// Adds two values and stores the result in the first value.
	/// @param lhs Quaternion on the left-hand side.
	/// @param rhs Scalar on the right-hand side.
	/// @return Reference to the quaternion on the left-hand side.
	/// @relates quaternion
	template <class T>
	inline constexpr quat<T>& operator+=(quat<T>& lhs, T rhs) noexcept
	{
		return lhs = lhs + rhs;
	}

	/// Subtracts the first value by the second value and stores the result in the first value.
	/// @param lhs Quaternion on the left-hand side.
	/// @param rhs Quaternion on the right-hand side.
	/// @return Reference to the quaternion on the left-hand side.
	/// @relates quaternion
	template <class T>
	inline constexpr quat<T>& operator-=(quat<T>& lhs, const quat<T>& rhs) noexcept
	{
		return lhs = lhs - rhs;
	}

	/// Subtracts the first value by the second value and stores the result in the first value.
	/// @param lhs Quaternion on the left-hand side.
	/// @param rhs Scalar on the right-hand side.
	/// @return Reference to the quaternion on the left-hand side.
	/// @relates quaternion
	template <class T>
	inline constexpr quat<T>& operator-=(quat<T>& lhs, T rhs) noexcept
	{
		return lhs = lhs - rhs;
	}

	/// Multiplies two values and stores the result in the first value.
	/// @param lhs Quaternion on the left-hand side.
	/// @param rhs Quaternion on the right-hand side.
	/// @return Reference to the quaternion on the left-hand side.
	/// @relates quaternion
	template <class T>
	inline constexpr quat<T>& operator*=(quat<T>& lhs, const quat<T>& rhs) noexcept
	{
		return lhs = lhs * rhs;
	}

	/// Multiplies two values and stores the result in the first value.
	/// @param lhs Quaternion on the left-hand side.
	/// @param rhs Scalar on the right-hand side.
	/// @return Reference to the quaternion on the left-hand side.
	/// @relates quaternion
	template <class T>
	inline constexpr quat<T>& operator*=(quat<T>& lhs, T rhs) noexcept
	{
		return lhs = lhs * rhs;
	}

	/// Divides the first value by the second value and stores the result in the first value.
	/// @param lhs Quaternion on the left-hand side.
	/// @param rhs Quaternion on the right-hand side.
	/// @return Reference to the quaternion on the left-hand side.
	/// @relates quaternion
	template <class T>
	inline constexpr quat<T>& operator/=(quat<T>& lhs, const quat<T>& rhs) noexcept
	{
		return lhs = lhs / rhs;
	}

	/// Divides the first value by the second value and stores the result in the first value.
	/// @param lhs Quaternion on the left-hand side.
	/// @param rhs Scalar on the right-hand side.
	/// @return Reference to the quaternion on the left-hand side.
	/// @relates quaternion
	template <class T>
	inline constexpr quat<T>& operator/=(quat<T>& lhs, T rhs) noexcept
	{
		return lhs = lhs / rhs;
	}

	/// @}
}

namespace engine::math::inline functions
{
	/// Calculates the conjugate of a quaternion.
	/// @param q Quaternion from which the conjugate will be calculated.
	/// @return Conjugate of the quaternion.
	template <class T>
	[[nodiscard]] inline constexpr quat<T> conjugate(const quat<T>& q) noexcept
	{
		return {q.r, -q.i};
	}

	/// Calculates the dot product of two quaternions.
	/// @param a First quaternion.
	/// @param b Second quaternion.
	/// @return Dot product of the two quaternions.
	template <class T>
	[[nodiscard]] inline constexpr T dot(const quat<T>& a, const quat<T>& b) noexcept
	{
		return a.r * b.r + dot(a.i, b.i);
	}

	/// Calculates the square length of a quaternion. The square length can be calculated faster than the length because a call to `sqrt` is saved.
	/// @param q Quaternion to calculate the square length of.
	/// @return Square length of the quaternion.
	template <class T>
	[[nodiscard]] inline constexpr T sqr_length(const quat<T>& q) noexcept
	{
		return sqr(q.r) + sqr_length(q.i);
	}

	/// Calculates the length of a quaternion.
	/// @param q Quaternion to calculate the length of.
	/// @return Length of the quaternion.
	template <class T>
	[[nodiscard]] inline T length(const quat<T>& q) noexcept
	{
		return sqrt(sqr_length(q));
	}

	/// Calculates the reciprocal length of a quaternion.
	/// @param q Quaternion to calculate the inverse length of.
	/// @return Inverse length of the quaternion.
	template <class T>
	[[nodiscard]] inline T rcp_length(const quat<T>& q) noexcept
	{
		return T{1} / length(q);
	}

	/// Normalizes a quaternion.
	/// @param q Quaternion to normalize.
	/// @return Unit quaternion.
	template <class T>
	[[nodiscard]] inline quat<T> normalize(const quat<T>& q) noexcept
	{
		return q * rcp_length(q);
	}

	/// Performs linear interpolation between two quaternions.
	/// @param a First quaternion.
	/// @param b Second quaternion.
	/// @param t Interpolation factor.
	/// @return Interpolated quaternion.
	template <class T>
	[[nodiscard]] inline constexpr quat<T> lerp(const quat<T>& a, const quat<T>& b, T t) noexcept
	{
		return {lerp(a.r, b.r, t), lerp(a.i, b.i, t)};
	}

	/// Performs normalized linear interpolation between two quaternions.
	/// @param a First quaternion.
	/// @param b Second quaternion.
	/// @param t Interpolation factor.
	/// @return Interpolated unit quaternion.
	template <class T>
	[[nodiscard]] inline quat<T> nlerp(const quat<T>& a, const quat<T>& b, T t) noexcept
	{
		return normalize(lerp(a, b * sign(dot(a, b)), t));
	}

	/// Performs spherical linear interpolation between two quaternions.
	/// @param a First unit quaternion.
	/// @param b Second unit quaternion.
	/// @param t Interpolation factor.
	/// @param tolerance Floating-point tolerance.
	/// @return Interpolated unit quaternion.
	template <class T>
	[[nodiscard]] quat<T> slerp(const quat<T>& a, quat<T> b, T t, T tolerance = T{1e-6}) noexcept
	{
		T cos_theta0 = dot(a, b);

		if (cos_theta0 < T{0})
		{
			// Negate one quaternion to take the shorter path
			cos_theta0 = -cos_theta0;
			b = -b;
		}

		if (cos_theta0 >= T{1} - tolerance)
		{
			// Quaternions are nearly aligned, use normalized linear interpolation
			return nlerp(a, b, t);
		}

		const T theta0 = acos(cos_theta0); // Angle between a and b
		const T theta1 = theta0 * t;       // Angle between a and result
		const quat<T> c = normalize(b - a * cos_theta0); // Quaternion orthogonal to a

		return a * cos(theta1) + c * sin(theta1);
	}

	/// Constructs a quaternion representing the minimum rotation from one direction to another direction.
	/// @param from Unit vector pointing in the source direction.
	/// @param to Unit vector pointing in the target direction.
	/// @param tolerance Floating-point tolerance.
	/// @return Unit quaternion representing the minimum rotation from direction @p from to direction @p to.
	/// @warning @p from and @p to must be unit vectors.
	template <class T>
	[[nodiscard]] quat<T> rotation(const vec3<T>& from, const vec3<T>& to, T tolerance = T{1e-6}) noexcept
	{
		const auto cos_theta = dot(from, to);

		if (cos_theta <= T{-1} + tolerance)
		{
			// Vectors are opposing, return 180 degree rotation about an arbitrary orthogonal axis
			return quat<T>{T{0}, basis_from_vec(from)[1]};
		}
		else if (cos_theta >= T{1} - tolerance)
		{
			// Vectors are codirectional, return identity quaternion
			return identity<quat<T>>;
		}
		else
		{
			const auto r = cos_theta + T{1};
			const auto i = cross(from, to);
			const auto rcp_length = rcp_sqrt(r * T{2});
			return quat<T>{r * rcp_length, i * rcp_length};
		}
	}

	/// Constructs a quaternion representing an angle-limited rotation from one direction towards another direction.
	/// @param from Unit vector pointing in the source direction.
	/// @param to Unit vector pointing in the target direction.
	/// @param max_angle Maximum angle of rotation, in radians.
	/// @param tolerance Floating-point tolerance.
	/// @return Unit quaternion representing a rotation from direction @p from towards direction @p to.
	/// @warning @p from and @p to must be unit vectors.
	template <class T>
	[[nodiscard]] quat<T> rotate_towards(const vec3<T>& from, const vec3<T>& to, T max_angle, T tolerance = T{1e-6}) noexcept
	{
		const auto cos_theta = dot(from, to);

		if (cos_theta <= T{-1} + tolerance)
		{
			// Vectors are opposing, return max angle (clamped to Pi) rotation about an arbitrary orthogonal axis
			return axis_angle_to_quat(basis_from_vec(from)[1], min(pi<T>, max_angle));
		}
		else if (cos_theta >= T{1} - tolerance)
		{
			// Vectors are codirectional, return identity quaternion
			return identity<quat<T>>;
		}
		else
		{
			const auto angle = acos(cos_theta);
			const auto axis = normalize(cross(from, to));
			return axis_angle_to_quat(axis, min(max_angle, angle));
		}
	}

	/// Decomposes a quaternion into swing and twist rotation components.
	/// @param q Unit quaternion to decompose.
	/// @param twist_axis Unit vector describing the axis of twist rotation.
	/// @param tolerance Floating-point tolerance.
	/// @return Array containing the swing rotation component, followed by the twist rotation component.
	template <class T>
	[[nodiscard]] std::array<quat<T>, 2> swing_twist(const quat<T>& q, const vec3<T>& twist_axis, T tolerance = T{1e-6}) noexcept
	{
		// Project rotation axis onto twist axis
		quat<T> twist = {q.r, project(q.i, twist_axis)};
		const T sqr_length_twist = sqr_length(twist);

		if (sqr_length_twist <= tolerance)
		{
			// No twist component
			return {q, identity<quat<T>>};
		}
		else
		{
			// Normalize twist and preserve sign of rotation
			twist *= sign(dot(twist.i, twist_axis)) / sqrt(sqr_length_twist);

			// Derive swing by removing twist from original rotation
			quat<T> swing = normalize(q * conjugate(twist));

			return {std::move(swing), std::move(twist)};
		}
	}
}

// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <engine/math/vector.hpp>
#include <engine/math/matrix-type.hpp>
#include <engine/math/quaternion-type.hpp>
#include <engine/math/functions.hpp>

namespace engine::math
{
	/// @name Basis Functions
	/// @{

	/// Constructs an orthonormal basis from a single unit vector.
	/// @param v First basis vector, must be a unit vector.
	/// @return Rotation matrix with column vectors that form the orthonormal basis defined by @p v. The first column vector will be @p v.
	/// @see Duff, Tom, et al. "Building an orthonormal basis, revisited." JCGT 6.1 (2017).
	template <class T>
	[[nodiscard]] constexpr mat3<T> basis_from_vec(const vec3<T>& v) noexcept
	{
		const T s = sign(v.z());
		const T a = T{-1} / (s + v.z());
		const T b = v.x() * v.y() * a;

		return
		{
			v,
			{T{1} + s * sqr(v.x()) * a, s * b, -s * v.x()},
			{b, s + sqr(v.y()) * a, -v.y()}
		};
	}

	/// Constructs an orthonormal basis from a unit quaternion.
	/// @param q Unit quaternion representing a rotation from world space to local space.
	/// @return Rotation matrix with column vectors that form the orthonormal basis defined by the quaternion.
	template <class T>
	[[nodiscard]] constexpr mat3<T> basis_from_quat(const quat<T>& q) noexcept
	{
		const T xx = q.x() * q.x();
		const T xy = q.x() * q.y();
		const T xz = q.x() * q.z();
		const T xw = q.x() * q.w();
		const T yy = q.y() * q.y();
		const T yz = q.y() * q.z();
		const T yw = q.y() * q.w();
		const T zz = q.z() * q.z();
		const T zw = q.z() * q.w();

		return
		{{
			{T{1} - (yy + zz) * T{2}, (xy + zw) * T{2}, (xz - yw) * T{2}},
			{(xy - zw) * T{2}, T{1} - (xx + zz) * T{2}, (yz + xw) * T{2}},
			{(xz + yw) * T{2}, (yz - xw) * T{2}, T{1} - (xx + yy) * T{2}}
		}};
	}

	/// Constructs a quaternion from orthonormal basis vectors.
	/// @param x X-axis of an orthonormal basis.
	/// @param y Y-axis of an orthonormal basis.
	/// @param z Z-axis of an orthonormal basis.
	/// @return Quaternion representing a rotation from world space to the local space defined by the basis vectors.
	template <class T>
	[[nodiscard]] quat<T> basis_to_quat(const vec3<T>& x, const vec3<T>& y, const vec3<T>& z) noexcept
	{
		quat<T> result;
		
		const T t = x.x() + y.y() + z.z();
		if (t > T{0})
		{
			const T s = T{0.5} / sqrt(T{1} + t);
			result =
			{
				T{0.25} / s,
				(y.z() - z.y()) * s,
				(z.x() - x.z()) * s,
				(x.y() - y.x()) * s
			};
		}
		else
		{
			if (x.x() > y.y() && x.x() > z.z())
			{
				const T s = T{2} * sqrt(T{1} + x.x() - y.y() - z.z());
				result =
				{
					(y.z() - z.y()) / s,
					T{0.25} * s,
					(y.x() + x.y()) / s,
					(z.x() + x.z()) / s
				};
			}
			else if (y.y() > z.z())
			{
				const T s = T{2} * sqrt(T{1} + y.y() - x.x() - z.z());
				result =
				{
					(z.x() - x.z()) / s,
					(y.x() + x.y()) / s,
					T{0.25} * s,
					(z.y() + y.z()) / s
				};
			}
			else
			{
				const T s = T{2} * sqrt(T{1} + z.z() - x.x() - y.y());
				result =
				{
					(x.y() - y.x()) / s,
					(z.x() + x.z()) / s,
					(z.y() + y.z()) / s,
					T{0.25} * s
				};
			}
		}

		return normalize(result);
	}

	/// Constructs a quaternion from orthonormal basis vectors.
	/// @param basis Rotation matrix with column vectors that form an orthonormal basis.
	/// @return Quaternion representing a rotation from world space to the local space defined by the basis vectors.
	template <class T>
	[[nodiscard]] inline quat<T> basis_to_quat(const mat3<T>& basis) noexcept
	{
		return basis_to_quat(basis[0], basis[1], basis[2]);
	}

	/// Constructs a quaternion from Y and Z orthonormal basis vectors, for a left-handed coordinate system.
	/// @param y Y-axis of an orthonormal basis.
	/// @param z Z-axis of an orthonormal basis.
	/// @return Quaternion representing a rotation from world space to the local space defined by the basis vectors, in a left-handed coordinate system.
	template <class T>
	[[nodiscard]] inline quat<T> basis_lh_to_quat(const vec3<T>& y, const vec3<T>& z) noexcept
	{
		const vec3<T> x = normalize(cross(y, z));
		return basis_to_quat(x, cross(x, z), z);
	}

	/// Constructs a quaternion from Y and Z orthonormal basis vectors, for a right-handed coordinate system.
	/// @param y Y-axis of an orthonormal basis.
	/// @param z Z-axis of an orthonormal basis.
	/// @return Quaternion representing a rotation from world space to the local space defined by the basis vectors, in a right-handed coordinate system.
	template <class T>
	[[nodiscard]] inline quat<T> basis_rh_to_quat(const vec3<T>& y, const vec3<T>& z) noexcept
	{
		const vec3<T> x = normalize(cross(z, y));
		return basis_to_quat(x, cross(x, z), -z);
	}

	/// @}
}

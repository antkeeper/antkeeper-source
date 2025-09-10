// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <engine/math/vector.hpp>
#include <engine/math/quaternion.hpp>
#include <engine/math/functions.hpp>
#include <engine/math/constants.hpp>
#include <concepts>

namespace engine::math
{
	/// @name Euler Angles
	/// @{

	/// Rotation sequences of proper Euler and Tait-Bryan angles.
	/// @details Indices of the first, second, and third rotation axes are encoded in bits 0-1, 2-3, and 4-5, respectively.
	enum class rotation_sequence
	{
		/// *z-x-z* rotation sequence (proper Euler angles).
		zxz = 0b100010,

		/// *x-y-x* rotation sequence (proper Euler angles).
		xyx = 0b000100,

		/// *y-z-y* rotation sequence (proper Euler angles).
		yzy = 0b011001,

		/// *z-y-z* rotation sequence (proper Euler angles).
		zyz = 0b100110,

		/// *x-z-x* rotation sequence (proper Euler angles).
		xzx = 0b001000,

		/// *y-x-y* rotation sequence (proper Euler angles).
		yxy = 0b010001,

		/// *x-y-z* rotation sequence (Tait-Bryan angles).
		xyz = 0b100100,

		/// *y-z-x* rotation sequence (Tait-Bryan angles).
		yzx = 0b001001,

		/// *z-x-y* rotation sequence (Tait-Bryan angles).
		zxy = 0b010010,

		/// *x-z-y* rotation sequence (Tait-Bryan angles).
		xzy = 0b011000,

		/// *z-y-x* rotation sequence (Tait-Bryan angles).
		zyx = 0b000110,

		/// *y-x-z* rotation sequence (Tait-Bryan angles).
		yxz = 0b100001
	};

	/// Returns the indices of the axes of a rotation sequence.
	/// @tparam T Index type.
	/// @param sequence Rotation sequence.
	/// @return Vector containing the indices of the axes of @p sequence.
	template <std::integral T>
	[[nodiscard]] inline constexpr vec3<T> rotation_axes(rotation_sequence sequence) noexcept
	{
		const auto x = static_cast<T>(sequence);
		return {x & 3, (x >> 2) & 3, x >> 4};
	}

	/// Derives Euler angles from a unit quaternion.
	/// @tparam Sequence Rotation sequence of the Euler angles.
	/// @tparam T Scalar type.
	/// @param q Quaternion to convert.
	/// @param tolerance Floating-point tolerance, in radians, for handling singularities.
	/// @return Euler angles, in radians.
	/// @see Bernardes, Evandro & Viollet, Stéphane. (2022). Quaternion to Euler angles conversion: A direct, general and computationally efficient method. PLoS ONE. 17. 10.1371/journal.pone.0276302.
	template <rotation_sequence Sequence, class T>
	[[nodiscard]] vec3<T> euler_from_quat(const quat<T>& q, T tolerance = T{1e-6})
	{
		constexpr auto axes = rotation_axes<int>(Sequence);
		constexpr auto proper = axes[0] == axes[2];
		constexpr auto i = axes[0];
		constexpr auto j = axes[1];
		constexpr auto k = proper ? 3 - i - j : axes[2];
		constexpr auto sign = static_cast<T>(((i - j) * (j - k) * (k - i)) >> 1);

		auto a = q.r;
		auto b = q.i[i];
		auto c = q.i[j];
		auto d = q.i[k] * sign;
		if constexpr (!proper)
		{
			a -= q.i[j];
			b += d;
			c += q.r;
			d -= q.i[i];
		}
		const auto aa_bb = a * a + b * b;

		vec3<T> angles;
		angles[1] = acos(T{2} *aa_bb / (aa_bb + c * c + d * d) - T{1});

		if (abs(angles[1]) <= tolerance)
		{
			angles[0] = T{0};
			angles[2] = atan(b, a) * T { 2 };
		}
		else if (abs(angles[1] - pi<T>) <= tolerance)
		{
			angles[0] = T{0};
			angles[2] = atan(-d, c) * T { -2 };
		}
		else
		{
			const auto half_sum = atan(b, a);
			const auto half_diff = atan(-d, c);

			angles[0] = half_sum + half_diff;
			angles[2] = half_sum - half_diff;
		}

		if constexpr (!proper)
		{
			angles[2] *= sign;
			angles[1] -= half_pi<T>;
		}

		return angles;
	}

	/// Derives Euler angles from a unit quaternion using the *z-x-z* rotation sequence.
	/// @tparam T Scalar type.
	/// @param q Quaternion to convert.
	/// @param tolerance Floating-point tolerance, in radians, for handling singularities.
	/// @return Euler angles, in radians.
	template <class T>
	[[nodiscard]] inline vec3<T> euler_zxz_from_quat(const quat<T>& q, T tolerance = T{1e-6})
	{
		return euler_from_quat<rotation_sequence::zxz, T>(q, tolerance);
	}

	/// Derives Euler angles from a unit quaternion using the *x-y-z* rotation sequence.
	/// @tparam T Scalar type.
	/// @param q Quaternion to convert.
	/// @param tolerance Floating-point tolerance, in radians, for handling singularities.
	/// @return Euler angles, in radians.
	template <class T>
	[[nodiscard]] inline vec3<T> euler_xyx_from_quat(const quat<T>& q, T tolerance = T{1e-6})
	{
		return euler_from_quat<rotation_sequence::xyx, T>(q, tolerance);
	}

	/// Derives Euler angles from a unit quaternion using the *y-z-y* rotation sequence.
	/// @tparam T Scalar type.
	/// @param q Quaternion to convert.
	/// @param tolerance Floating-point tolerance, in radians, for handling singularities.
	/// @return Euler angles, in radians.
	template <class T>
	[[nodiscard]] inline vec3<T> euler_yzy_from_quat(const quat<T>& q, T tolerance = T{1e-6})
	{
		return euler_from_quat<rotation_sequence::yzy, T>(q, tolerance);
	}

	/// Derives Euler angles from a unit quaternion using the *z-y-z* rotation sequence.
	/// @tparam T Scalar type.
	/// @param q Quaternion to convert.
	/// @param tolerance Floating-point tolerance, in radians, for handling singularities.
	/// @return Euler angles, in radians.
	template <class T>
	[[nodiscard]] inline vec3<T> euler_zyz_from_quat(const quat<T>& q, T tolerance = T{1e-6})
	{
		return euler_from_quat<rotation_sequence::zyz, T>(q, tolerance);
	}

	/// Derives Euler angles from a unit quaternion using the *x-z-x* rotation sequence.
	/// @tparam T Scalar type.
	/// @param q Quaternion to convert.
	/// @param tolerance Floating-point tolerance, in radians, for handling singularities.
	/// @return Euler angles, in radians.
	template <class T>
	[[nodiscard]] inline vec3<T> euler_xzx_from_quat(const quat<T>& q, T tolerance = T{1e-6})
	{
		return euler_from_quat<rotation_sequence::xzx, T>(q, tolerance);
	}

	/// Derives Euler angles from a unit quaternion using the *y-x-y* rotation sequence.
	/// @tparam T Scalar type.
	/// @param q Quaternion to convert.
	/// @param tolerance Floating-point tolerance, in radians, for handling singularities.
	/// @return Euler angles, in radians.
	template <class T>
	[[nodiscard]] inline vec3<T> euler_yxy_from_quat(const quat<T>& q, T tolerance = T{1e-6})
	{
		return euler_from_quat<rotation_sequence::yxy, T>(q, tolerance);
	}

	/// Derives Euler angles from a unit quaternion using the *x-y-z* rotation sequence.
	/// @tparam T Scalar type.
	/// @param q Quaternion to convert.
	/// @param tolerance Floating-point tolerance, in radians, for handling singularities.
	/// @return Euler angles, in radians.
	template <class T>
	[[nodiscard]] inline vec3<T> euler_xyz_from_quat(const quat<T>& q, T tolerance = T{1e-6})
	{
		return euler_from_quat<rotation_sequence::xyz, T>(q, tolerance);
	}

	/// Derives Euler angles from a unit quaternion using the *y-z-x* rotation sequence.
	/// @tparam T Scalar type.
	/// @param q Quaternion to convert.
	/// @param tolerance Floating-point tolerance, in radians, for handling singularities.
	/// @return Euler angles, in radians.
	template <class T>
	[[nodiscard]] inline vec3<T> euler_yzx_from_quat(const quat<T>& q, T tolerance = T{1e-6})
	{
		return euler_from_quat<rotation_sequence::yzx, T>(q, tolerance);
	}

	/// Derives Euler angles from a unit quaternion using the *z-x-y* rotation sequence.
	/// @tparam T Scalar type.
	/// @param q Quaternion to convert.
	/// @param tolerance Floating-point tolerance, in radians, for handling singularities.
	/// @return Euler angles, in radians.
	template <class T>
	[[nodiscard]] inline vec3<T> euler_zxy_from_quat(const quat<T>& q, T tolerance = T{1e-6})
	{
		return euler_from_quat<rotation_sequence::zxy, T>(q, tolerance);
	}

	/// Derives Euler angles from a unit quaternion using the *x-z-y* rotation sequence.
	/// @tparam T Scalar type.
	/// @param q Quaternion to convert.
	/// @param tolerance Floating-point tolerance, in radians, for handling singularities.
	/// @return Euler angles, in radians.
	template <class T>
	[[nodiscard]] inline vec3<T> euler_xzy_from_quat(const quat<T>& q, T tolerance = T{1e-6})
	{
		return euler_from_quat<rotation_sequence::xzy, T>(q, tolerance);
	}

	/// Derives Euler angles from a unit quaternion using the *z-y-x* rotation sequence.
	/// @tparam T Scalar type.
	/// @param q Quaternion to convert.
	/// @param tolerance Floating-point tolerance, in radians, for handling singularities.
	/// @return Euler angles, in radians.
	template <class T>
	[[nodiscard]] inline vec3<T> euler_zyx_from_quat(const quat<T>& q, T tolerance = T{1e-6})
	{
		return euler_from_quat<rotation_sequence::zyx, T>(q, tolerance);
	}

	/// Derives Euler angles from a unit quaternion using the *y-x-z* rotation sequence.
	/// @tparam T Scalar type.
	/// @param q Quaternion to convert.
	/// @param tolerance Floating-point tolerance, in radians, for handling singularities.
	/// @return Euler angles, in radians.
	template <class T>
	[[nodiscard]] inline vec3<T> euler_yxz_from_quat(const quat<T>& q, T tolerance = T{1e-6})
	{
		return euler_from_quat<rotation_sequence::yxz, T>(q, tolerance);
	}

	/// Derives Euler angles from a unit quaternion.
	/// @tparam T Scalar type.
	/// @param sequence Rotation sequence of the Euler angles.
	/// @param q Quaternion to convert.
	/// @param tolerance Floating-point tolerance, in radians, for handling singularities.
	/// @return Euler angles, in radians.
	template <class T>
	[[nodiscard]] inline vec3<T> euler_from_quat(rotation_sequence sequence, const quat<T>& q, T tolerance = T{1e-6})
	{
		switch (sequence)
		{
			case rotation_sequence::zxz:
				return euler_zxz_from_quat<T>(q, tolerance);
			case rotation_sequence::xyx:
				return euler_xyx_from_quat<T>(q, tolerance);
			case rotation_sequence::yzy:
				return euler_yzy_from_quat<T>(q, tolerance);
			case rotation_sequence::zyz:
				return euler_zyz_from_quat<T>(q, tolerance);
			case rotation_sequence::xzx:
				return euler_xzx_from_quat<T>(q, tolerance);
			case rotation_sequence::yxy:
				return euler_yxy_from_quat<T>(q, tolerance);
			case rotation_sequence::xyz:
				return euler_xyz_from_quat<T>(q, tolerance);
			case rotation_sequence::yzx:
				return euler_yzx_from_quat<T>(q, tolerance);
			case rotation_sequence::zxy:
				return euler_zxy_from_quat<T>(q, tolerance);
			case rotation_sequence::xzy:
				return euler_xzy_from_quat<T>(q, tolerance);
			case rotation_sequence::zyx:
				return euler_zyx_from_quat<T>(q, tolerance);
			case rotation_sequence::yxz:
			default:
				return euler_yxz_from_quat<T>(q, tolerance);
		}
	}

	/// Constructs a quaternion from Euler angles.
	/// @tparam Sequence Rotation sequence of the Euler angles.
	/// @tparam T Scalar type.
	/// @param angles Vector of Euler angles.
	/// @return Quaternion.
	/// @see Diebel, J. (2006). Representing attitude: Euler angles, unit quaternions, and rotation vectors. Matrix, 58(15-16), 1-35.
	template <rotation_sequence Sequence, class T>
	[[nodiscard]] quat<T> euler_to_quat(const vec3<T>& angles)
	{
		const auto half_angles = angles * T{0.5};
		const auto cx = cos(half_angles.x());
		const auto sx = sin(half_angles.x());
		const auto cy = cos(half_angles.y());
		const auto sy = sin(half_angles.y());
		const auto cz = cos(half_angles.z());
		const auto sz = sin(half_angles.z());

		if constexpr (Sequence == rotation_sequence::zxz)
		{
			return
			{
				cx * cy * cz - sx * cy * sz,
				cx * cz * sy + sx * sy * sz,
				cx * sy * sz - sx * cz * sy,
				cx * cy * sz + cy * cz * sx
			};
		}
		else if constexpr (Sequence == rotation_sequence::xyx)
		{
			return
			{
				cx * cy * cz - sx * cy * sz,
				cx * cy * sz + cy * cz * sx,
				cx * cz * sy + sx * sy * sz,
				cx * sy * sz - sx * cz * sy
			};
		}
		else if constexpr (Sequence == rotation_sequence::yzy)
		{
			return
			{
				cx * cy * cz - sx * cy * sz,
				cx * sy * sz - sx * cz * sy,
				cx * cy * sz + cy * cz * sx,
				cx * cz * sy + sx * sy * sz
			};
		}
		else if constexpr (Sequence == rotation_sequence::zyz)
		{
			return
			{
				cx * cy * cz - sx * cy * sz,
				-cx * sy * sz + sx * cz * sy,
				cx * cz * sy + sx * sy * sz,
				cx * cy * sz + cy * cz * sx
			};
		}
		else if constexpr (Sequence == rotation_sequence::xzx)
		{
			return
			{
				cx * cy * cz - sx * cy * sz,
				cx * cy * sz + cy * cz * sx,
				-cx * sy * sz + sx * cz * sy,
				cx * cz * sy + sx * sy * sz
			};
		}
		else if constexpr (Sequence == rotation_sequence::yxy)
		{
			return
			{
				cx * cy * cz - sx * cy * sz,
				cx * cz * sy + sx * sy * sz,
				cx * cy * sz + cy * cz * sx,
				-cx * sy * sz + sx * cz * sy
			};
		}
		else if constexpr (Sequence == rotation_sequence::xyz)
		{
			return
			{
				cx * cy * cz + sx * sy * sz,
				-cx * sy * sz + cy * cz * sx,
				cx * cz * sy + sx * cy * sz,
				cx * cy * sz - sx * cz * sy
			};
		}
		else if constexpr (Sequence == rotation_sequence::yzx)
		{
			return
			{
				cx * cy * cz + sx * sy * sz,
				cx * cy * sz - sx * cz * sy,
				-cx * sy * sz + cy * cz * sx,
				cx * cz * sy + sx * cy * sz
			};
		}
		else if constexpr (Sequence == rotation_sequence::zxy)
		{
			return
			{
				cx * cy * cz + sx * sy * sz,
				cx * cz * sy + sx * cy * sz,
				cx * cy * sz - sx * cz * sy,
				-cx * sy * sz + cy * cz * sx
			};
		}
		else if constexpr (Sequence == rotation_sequence::xzy)
		{
			return
			{
				cx * cy * cz - sx * sy * sz,
				cx * sy * sz + cy * cz * sx,
				cx * cy * sz + sx * cz * sy,
				cx * cz * sy - sx * cy * sz
			};
		}
		else if constexpr (Sequence == rotation_sequence::zyx)
		{
			return
			{
				cx * cy * cz - sx * sy * sz,
				cx * cy * sz + sx * cz * sy,
				cx * cz * sy - sx * cy * sz,
				cx * sy * sz + cy * cz * sx
			};
		}
		else //if constexpr (Sequence == rotation_sequence::yxz)
		{
			return
			{
				cx * cy * cz - sx * sy * sz,
				cx * cz * sy - sx * cy * sz,
				cx * sy * sz + cy * cz * sx,
				cx * cy * sz + sx * cz * sy
			};
		}
	}

	/// Constructs a quaternion from Euler angles using the *z-x-z* rotation sequence.
	/// @tparam T Scalar type.
	/// @param angles Vector of Euler angles.
	/// @return Quaternion.
	template <class T>
	[[nodiscard]] inline quat<T> euler_zxz_to_quat(const vec3<T>& angles)
	{
		return euler_to_quat<rotation_sequence::zxz, T>(angles);
	}

	/// Constructs a quaternion from Euler angles using the *x-y-x* rotation sequence.
	/// @tparam T Scalar type.
	/// @param angles Vector of Euler angles.
	/// @return Quaternion.
	template <class T>
	[[nodiscard]] inline quat<T> euler_xyx_to_quat(const vec3<T>& angles)
	{
		return euler_to_quat<rotation_sequence::xyx, T>(angles);
	}

	/// Constructs a quaternion from Euler angles using the *y-z-y* rotation sequence.
	/// @tparam T Scalar type.
	/// @param angles Vector of Euler angles.
	/// @return Quaternion.
	template <class T>
	[[nodiscard]] inline quat<T> euler_yzy_to_quat(const vec3<T>& angles)
	{
		return euler_to_quat<rotation_sequence::yzy, T>(angles);
	}

	/// Constructs a quaternion from Euler angles using the *z-y-z* rotation sequence.
	/// @tparam T Scalar type.
	/// @param angles Vector of Euler angles.
	/// @return Quaternion.
	template <class T>
	[[nodiscard]] inline quat<T> euler_zyz_to_quat(const vec3<T>& angles)
	{
		return euler_to_quat<rotation_sequence::zyz, T>(angles);
	}

	/// Constructs a quaternion from Euler angles using the *x-z-x* rotation sequence.
	/// @tparam T Scalar type.
	/// @param angles Vector of Euler angles.
	/// @return Quaternion.
	template <class T>
	[[nodiscard]] inline quat<T> euler_xzx_to_quat(const vec3<T>& angles)
	{
		return euler_to_quat<rotation_sequence::xzx, T>(angles);
	}

	/// Constructs a quaternion from Euler angles using the *y-x-y* rotation sequence.
	/// @tparam T Scalar type.
	/// @param angles Vector of Euler angles.
	/// @return Quaternion.
	template <class T>
	[[nodiscard]] inline quat<T> euler_yxy_to_quat(const vec3<T>& angles)
	{
		return euler_to_quat<rotation_sequence::yxy, T>(angles);
	}

	/// Constructs a quaternion from Euler angles using the *x-y-z* rotation sequence.
	/// @tparam T Scalar type.
	/// @param angles Vector of Euler angles.
	/// @return Quaternion.
	template <class T>
	[[nodiscard]] inline quat<T> euler_xyz_to_quat(const vec3<T>& angles)
	{
		return euler_to_quat<rotation_sequence::xyz, T>(angles);
	}

	/// Constructs a quaternion from Euler angles using the *y-z-x* rotation sequence.
	/// @tparam T Scalar type.
	/// @param angles Vector of Euler angles.
	/// @return Quaternion.
	template <class T>
	[[nodiscard]] inline quat<T> euler_yzx_to_quat(const vec3<T>& angles)
	{
		return euler_to_quat<rotation_sequence::yzx, T>(angles);
	}

	/// Constructs a quaternion from Euler angles using the *z-x-y* rotation sequence.
	/// @tparam T Scalar type.
	/// @param angles Vector of Euler angles.
	/// @return Quaternion.
	template <class T>
	[[nodiscard]] inline quat<T> euler_zxy_to_quat(const vec3<T>& angles)
	{
		return euler_to_quat<rotation_sequence::zxy, T>(angles);
	}

	/// Constructs a quaternion from Euler angles using the *x-z-y* rotation sequence.
	/// @tparam T Scalar type.
	/// @param angles Vector of Euler angles.
	/// @return Quaternion.
	template <class T>
	[[nodiscard]] inline quat<T> euler_xzy_to_quat(const vec3<T>& angles)
	{
		return euler_to_quat<rotation_sequence::xzy, T>(angles);
	}

	/// Constructs a quaternion from Euler angles using the *z-y-x* rotation sequence.
	/// @tparam T Scalar type.
	/// @param angles Vector of Euler angles.
	/// @return Quaternion.
	template <class T>
	[[nodiscard]] inline quat<T> euler_zyx_to_quat(const vec3<T>& angles)
	{
		return euler_to_quat<rotation_sequence::zyx, T>(angles);
	}

	/// Constructs a quaternion from Euler angles using the *y-x-z* rotation sequence.
	/// @tparam T Scalar type.
	/// @param angles Vector of Euler angles.
	/// @return Quaternion.
	template <class T>
	[[nodiscard]] inline quat<T> euler_yxz_to_quat(const vec3<T>& angles)
	{
		return euler_to_quat<rotation_sequence::yxz, T>(angles);
	}

	/// Constructs a quaternion from Euler angles.
	/// @tparam T Scalar type.
	/// @param sequence Rotation sequence of the Euler angles.
	/// @param angles Vector of Euler angles.
	/// @return Quaternion.
	template <class T>
	[[nodiscard]] inline quat<T> euler_to_quat(rotation_sequence sequence, const vec3<T>& angles)
	{
		switch (sequence)
		{
			case rotation_sequence::zxz:
				return euler_zxz_to_quat<T>(angles);
			case rotation_sequence::xyx:
				return euler_xyx_to_quat<T>(angles);
			case rotation_sequence::yzy:
				return euler_yzy_to_quat<T>(angles);
			case rotation_sequence::zyz:
				return euler_zyz_to_quat<T>(angles);
			case rotation_sequence::xzx:
				return euler_xzx_to_quat<T>(angles);
			case rotation_sequence::yxy:
				return euler_yxy_to_quat<T>(angles);
			case rotation_sequence::xyz:
				return euler_xyz_to_quat<T>(angles);
			case rotation_sequence::yzx:
				return euler_yzx_to_quat<T>(angles);
			case rotation_sequence::zxy:
				return euler_zxy_to_quat<T>(angles);
			case rotation_sequence::xzy:
				return euler_xzy_to_quat<T>(angles);
			case rotation_sequence::zyx:
				return euler_zyx_to_quat<T>(angles);
			case rotation_sequence::yxz:
			default:
				return euler_yxz_to_quat<T>(angles);
		}
	}

	/// @} Euler Angles
}

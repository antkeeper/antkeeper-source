// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <engine/math/vector.hpp>
#include <engine/math/matrix.hpp>
#include <engine/math/quaternion.hpp>
#include <engine/math/basis.hpp>

namespace engine::math
{
	inline namespace types
	{
		/// SRT transformation.
		/// @tparam T Scalar type.
		template <class T>
		struct transform
		{
			/// Scalar type.
			using scalar_type = T;

			/// Vector type.
			using vector_type = vec3<T>;

			/// Quaternion type.
			using quaternion_type = quat<T>;

			/// Transformation matrix type.
			using matrix_type = mat4<T>;

			/// Translation vector.
			vector_type translation;

			/// Rotation quaternion.
			quaternion_type rotation;

			/// Scale vector.
			vector_type scale;

			/// Constructs a matrix representing the transformation.
			/// @return Transformation matrix.
			[[nodiscard]] constexpr matrix_type matrix() const noexcept
			{
				auto matrix = mat4<T>(basis_from_quat(rotation));

				matrix[0][0] *= scale[0];
				matrix[0][1] *= scale[0];
				matrix[0][2] *= scale[0];

				matrix[1][0] *= scale[1];
				matrix[1][1] *= scale[1];
				matrix[1][2] *= scale[1];

				matrix[2][0] *= scale[2];
				matrix[2][1] *= scale[2];
				matrix[2][2] *= scale[2];

				matrix[3][0] = translation[0];
				matrix[3][1] = translation[1];
				matrix[3][2] = translation[2];

				return matrix;
			}

			/// @copydoc matrix() const
			[[nodiscard]] inline constexpr explicit operator matrix_type() const noexcept
			{
				return matrix();
			}

			/// Type-casts the transform scalars using `static_cast`.
			/// @tparam U Target scalar type.
			/// @return Type-casted transform.
			template <class U>
			[[nodiscard]] inline constexpr explicit operator transform<U>() const noexcept
			{
				return {vec3<U>(translation), quat<U>(rotation), vec3<U>(scale)};
			}
		};
	}

	inline namespace constants
	{
		/// @name Transform constants
		/// @{

		/// Identity transform.
		template <class T>
		inline constexpr transform<T> identity<transform<T>> = {zero<vec3<T>>, identity<quat<T>>, one<vec3<T>>};

		/// @}
	}

	inline namespace functions
	{
		/// Calculates the inverse of a transform.
		/// @param t Transform of which to take the inverse.
		template <class T>
		[[nodiscard]] transform<T> inverse(const transform<T>& t) noexcept;

		/// Combines two transforms.
		/// @param x First transform.
		/// @param y Second transform.
		/// @return Product of the two transforms.
		template <class T>
		[[nodiscard]] transform<T> mul(const transform<T>& x, const transform<T>& y);

		/// Transforms a vector by a transform.
		/// @param t Transform.
		/// @param v Vector.
		/// @return Transformed vector.
		template <class T>
		[[nodiscard]] constexpr vector<T, 3> mul(const transform<T>& t, const vector<T, 3>& v) noexcept;

		/// Transforms a vector by the inverse of a transform.
		/// @param v Vector.
		/// @param t Transform.
		/// @return Transformed vector.
		template <class T>
		[[nodiscard]] constexpr vector<T, 3> mul(const vector<T, 3>& v, const transform<T>& t) noexcept;

		template <class T>
		transform<T> inverse(const transform<T>& t) noexcept
		{
			return {-t.translation * t.rotation, conjugate(t.rotation), T{1} / t.scale};
		}

		template <class T>
		transform<T> mul(const transform<T>& x, const transform<T>& y)
		{
			return
			{
				mul(x, y.translation),
				normalize(x.rotation * y.rotation),
				x.scale * y.scale
			};
		}

		template <class T>
		constexpr vector<T, 3> mul(const transform<T>& t, const vector<T, 3>& v) noexcept
		{
			return t.translation + t.rotation * (t.scale * v);
		}

		template <class T>
		inline constexpr vector<T, 3> mul(const vector<T, 3>& v, const transform<T>& t) noexcept
		{
			return mul(inverse(t), v);
		}
	}
}

/// Combines two transforms.
/// @param x First transform.
/// @param y Second transform.
/// @return Product of the two transforms.
template <class T>
[[nodiscard]] inline engine::math::transform<T> operator*(const engine::math::transform<T>& x, const engine::math::transform<T>& y)
{
	return engine::math::mul(x, y);
}

/// Transforms a vector by a transform.
/// @param t Transform.
/// @param v Vector.
/// @return Transformed vector.
template <class T>
[[nodiscard]] inline constexpr engine::math::vec3<T> operator*(const engine::math::transform<T>& t, const engine::math::vec3<T>& v) noexcept
{
	return engine::math::mul(t, v);
}

/// Transforms a vector by the inverse of a transform.
/// @param v Vector.
/// @param t Transform.
/// @return Transformed vector.
template <class T>
[[nodiscard]] inline constexpr engine::math::vec3<T> operator*(const engine::math::vec3<T>& v, const engine::math::transform<T>& t) noexcept
{
	return engine::math::mul(v, t);
}

/// Combines two transforms and stores the result in the first transform.
/// @param x First transform.
/// @param y Second transform.
/// @return Reference to the first transform.
template <class T>
inline engine::math::transform<T>& operator*=(engine::math::transform<T>& x, const engine::math::transform<T>& y)
{
	return (x = x * y);
}

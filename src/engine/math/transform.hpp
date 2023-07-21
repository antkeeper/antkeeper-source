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

#ifndef ANTKEEPER_MATH_TRANSFORM_HPP
#define ANTKEEPER_MATH_TRANSFORM_HPP

#include <engine/math/vector.hpp>
#include <engine/math/quaternion.hpp>
#include <engine/math/matrix.hpp>

namespace math {

/**
 * 3D transformation.
 *
 * Transformations are applied in the following order: scale, rotation, translation.
 *
 * @tparam T Scalar type.
 */
template <class T>
struct transform
{
	/// Scalar type.
	using scalar_type = T;
	
	/// Vector type.
	using vector_type = vector<T, 3>;
	
	/// Quaternion type.
	using quaternion_type = quaternion<T>;
	
	/// Transformation matrix type.
	using matrix_type = matrix<T, 4, 4>;
	
	/// Translation vector.
	vector_type translation;
	
	/// Rotation quaternion.
	quaternion_type rotation;
	
	/// Scale vector.
	vector_type scale;
	
	/**
	 * Constructs a matrix representing the transformation.
	 *
	 * @return Transformation matrix.
	 */
	/// @{
	[[nodiscard]] constexpr explicit operator matrix_type() const noexcept
	{
		matrix_type matrix = matrix_type(quaternion_type::matrix_type(rotation));
		
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
	
	[[nodiscard]] inline constexpr matrix_type matrix() const noexcept
	{
		return matrix_type(*this);
	}
	/// @}
	
	/*
	 * Type-casts the transform scalars using `static_cast`.
	 *
	 * @tparam U Target scalar type.
	 *
	 * @return Type-casted transform.
	 */
	template <class U>
	[[nodiscard]] inline constexpr explicit operator transform<U>() const noexcept
	{
		return {math::vector<U, 3>(translation), math::quaternion<U>(rotation), math::vector<U, 3>(scale)};
	}
	
	/// Returns an identity transform.
	[[nodiscard]] static inline constexpr transform identity() noexcept
	{
		return {vector_type::zero(), quaternion_type::identity(), vector_type::one()};
	}
};

/**
 * Calculates the inverse of a transform.
 *
 * @param t Transform of which to take the inverse.
 */
template <class T>
[[nodiscard]] transform<T> inverse(const transform<T>& t) noexcept;

/**
 * Combines two transforms.
 *
 * @param x First transform.
 * @param y Second transform.
 *
 * @return Product of the two transforms.
 */
template <class T>
[[nodiscard]] transform<T> mul(const transform<T>& x, const transform<T>& y);

/**
 * Transforms a vector by a transform.
 *
 * @param t Transform.
 * @param v Vector.
 *
 * @return Transformed vector.
 */
template <class T>
[[nodiscard]] constexpr vector<T, 3> mul(const transform<T>& t, const vector<T, 3>& v) noexcept;

/**
 * Transforms a vector by the inverse of a transform.
 *
 * @param v Vector.
 * @param t Transform.
 *
 * @return Transformed vector.
 */
template <class T>
[[nodiscard]] constexpr vector<T, 3> mul(const vector<T, 3>& v, const transform<T>& t) noexcept;

template <class T>
transform<T> inverse(const transform<T>& t) noexcept
{
	transform<T> inverse_t;
	
	inverse_t.scale = T{1} / t.scale;
	inverse_t.rotation = conjugate(t.rotation);
	inverse_t.translation = -(inverse_t.rotation * t.translation);
	
	return inverse_t;
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

namespace operators {

/// @copydoc math::mul(const math::transform<T>&, const math::transform<T>&)
template <class T>
[[nodiscard]] inline math::transform<T> operator*(const math::transform<T>& x, const math::transform<T>& y)
{
	return mul(x, y);
}

/// @copydoc math::mul(const math::transform<T>&, const math::vec3<T>&)
template <class T>
[[nodiscard]] inline constexpr math::vec3<T> operator*(const math::transform<T>& t, const math::vec3<T>& v) noexcept
{
	return mul(t, v);
}

/// @copydoc math::mul(const math::vec3<T>&, const math::transform<T>&)
template <class T>
[[nodiscard]] inline constexpr math::vec3<T> operator*(const math::vec3<T>& v, const math::transform<T>& t) noexcept
{
	return mul(v, t);
}

/**
 * Combines two transforms and stores the result in the first transform.
 *
 * @param x First transform.
 * @param y Second transform.
 *
 * @return Reference to the first transform.
 */
template <class T>
inline math::transform<T>& operator*=(math::transform<T>& x, const math::transform<T>& y)
{
	return (x = x * y);
}

} // namespace operators

} // namespace math

// Bring transform operators into global namespace
using namespace math::operators;

#endif // ANTKEEPER_MATH_TRANSFORM_HPP

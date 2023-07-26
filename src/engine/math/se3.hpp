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

#ifndef ANTKEEPER_MATH_TRANSFORMATION_SE3_HPP
#define ANTKEEPER_MATH_TRANSFORMATION_SE3_HPP

#include <engine/math/vector.hpp>
#include <engine/math/quaternion.hpp>

namespace math {

/// Transformation types
namespace transformation_types {

/**
 * SE(3) proper rigid transformation (rototranslation).
 *
 * @tparam T Scalar type.
 */
template <class T>
struct se3
{
public:
	/// Scalar type.
	using scalar_type = T;
	
	/// Vector type.
	using vector_type = vec3<T>;
	
	/// Quaternion type.
	using quaternion_type = quat<T>;
	
	/// Transformation matrix type.
	using matrix_type = mat4<T>;
	
	/// Vector representing the translation component of the transformation.
	vector_type t;
	
	/// Quaternion representing the rotation component of the transformation.
	quaternion_type r;
	
	/// Returns the inverse of this transformation.
	[[nodiscard]] constexpr se3 inverse() const noexcept
	{
		const quaternion_type inverse_r = conjugate(r);
		const vector_type inverse_t = -(inverse_r * t);
		return {inverse_t, inverse_r};
	}
	
	/// Returns a matrix representation of this transformation.
	/// @{
	[[nodiscard]] constexpr matrix_type matrix() const noexcept
	{
		matrix_type m = mat4<T>(mat3<T>(r));
		
		m[3].x() = t.x();
		m[3].y() = t.y();
		m[3].z() = t.z();
		
		return m;
	}
	
	[[nodiscard]] inline constexpr explicit operator matrix_type() const noexcept
	{
		return matrix();
	}
	/// @}
	
	/**
	 * Transforms a vector by this transformation.
	 *
	 * @param v Untransformed vector.
	 *
	 * @return Transformed vector.
	 */
	/// @{
	[[nodiscard]] inline constexpr vector_type transform(const vector_type& v) const noexcept
	{
		return r * v + t;
	}
	
	[[nodiscard]] inline constexpr vector_type operator*(const vector_type& v) const noexcept
	{
		return transform(v);
	}
	/// @}
	
	/**
	 * Transforms an SE(3) transformation by this transformation.
	 *
	 * @param xf SE(3) transformation.
	 *
	 * @return Frame in this se3's space.
	 */
	/// @{
	[[nodiscard]] constexpr se3 transform(const se3& xf) const noexcept
	{
		return {xf.transform(t), normalize(xf.r * r)};
	}
	
	[[nodiscard]] inline constexpr se3 operator*(const se3& xf) const noexcept
	{
		return transform(xf);
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
	[[nodiscard]] inline constexpr explicit operator se3<U>() const noexcept
	{
		return {vec3<U>(t), quat<U>(r)};
	}
	
	/// Returns an identity transformation.
	[[nodiscard]] static inline constexpr se3 identity() noexcept
	{
		return {vector_type::zero(), quaternion_type::identity()};
	}
};

} // namespace transformation_types

// Bring transformation types into math namespace
using namespace transformation_types;

// Bring transformation types into math::types namespace
namespace types { using namespace math::transformation_types; }

} // namespace math

#endif // ANTKEEPER_MATH_TRANSFORMATION_SE3_HPP

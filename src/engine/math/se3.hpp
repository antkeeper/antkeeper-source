// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_MATH_SE3_HPP
#define ANTKEEPER_MATH_SE3_HPP

#include <engine/math/vector.hpp>
#include <engine/math/matrix.hpp>
#include <engine/math/quaternion.hpp>

// export module math.se3;
// import math.vector;
// import math.matrix;
// import math.quaternion;

namespace math {

/// SE(3) proper rigid transformation (rototranslation).
/// @tparam T Scalar type.
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
		return {-t * r, conjugate(r)};
	}
	
	/// Returns a matrix representation of this transformation.
	[[nodiscard]] constexpr matrix_type matrix() const noexcept
	{
		matrix_type m = mat4<T>(mat3<T>(r));
		
		m[3].x() = t.x();
		m[3].y() = t.y();
		m[3].z() = t.z();
		
		return m;
	}
	
	/// @copydoc matrix() const
	[[nodiscard]] inline constexpr explicit operator matrix_type() const noexcept
	{
		return matrix();
	}
	
	/// Transforms a vector by this transformation.
	/// @param v Untransformed vector.
	/// @return Transformed vector.
	[[nodiscard]] inline constexpr vector_type transform(const vector_type& v) const noexcept
	{
		return r * v + t;
	}
	
	/// @copydoc transform(const vector_type&) const
	[[nodiscard]] inline constexpr vector_type operator*(const vector_type& v) const noexcept
	{
		return transform(v);
	}
	
	/// Transforms an SE(3) transformation by this transformation.
	/// @param xf SE(3) transformation.
	/// @return Frame in this se3's space.
	[[nodiscard]] constexpr se3 transform(const se3& xf) const noexcept
	{
		return {xf.transform(t), normalize(xf.r * r)};
	}
	
	/// @copydoc transform(const se3&) const
	[[nodiscard]] inline constexpr se3 operator*(const se3& xf) const noexcept
	{
		return transform(xf);
	}
	
	/// Type-casts the transform scalars using `static_cast`.
	/// @tparam U Target scalar type.
	/// @return Type-casted transform.
	template <class U>
	[[nodiscard]] inline constexpr explicit operator se3<U>() const noexcept
	{
		return {vec3<U>(t), quat<U>(r)};
	}
};

namespace types
{
	using math::se3;
} // namespace types

namespace constants {

/// @name SE(3) constants
/// @{

/// Identity SE(3) transformation.
template <class T>
inline constexpr se3<T> identity<se3<T>> = {zero<vec3<T>>, identity<quat<T>>};

/// @}

} // namespace constants

// Bring math::constants into math namespace
using namespace constants;

} // namespace math

#endif // ANTKEEPER_MATH_SE3_HPP

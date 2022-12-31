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

#include "math/vector.hpp"
#include "math/quaternion.hpp"

namespace math {
namespace transformation {

/**
 * 3-dimensional Euclidean proper rigid transformation in SE(3).
 *
 * @tparam T Scalar type.
 */
template <class T>
struct se3
{
public:
	/// Scalar type.
	typedef T scalar_type;
	
	/// Vector type.
	typedef math::vector3<T> vector_type;
	
	/// Quaternion type.
	typedef math::quaternion<T> quaternion_type;
	
	/// Transformation matrix type.
	typedef math::matrix<T, 4, 4> matrix_type;
	
	/// Vector representing the translation component of this SE(3) transformation.
	vector_type t;
	
	/// Quaternion representing the rotation component of this SE(3) transformation.
	quaternion_type r;
	
	/// Returns the inverse of this SE(3) transformation.
	se3 inverse() const;
	
	/// Returns a matrix representation of the SE(3) transformation.
	matrix_type matrix() const;
	
	/**
	 * Transforms a vector by this SE(3) transformation.
	 *
	 * @param x Untransformed vector.
	 * @return Transformed vector.
	 */
	vector_type transform(const vector_type& x) const;
	
	/**
	 * Transforms an SE(3) transformation by this SE(3) transformation.
	 *
	 * @param x Other SE(3) transformation.
	 * @return Frame in this se3's space.
	 */
	se3 transform(const se3& x) const;
	
	/// @copydoc se3::transform(const vector_type&) const
	vector_type operator*(const vector_type& x) const;
	
	/// @copydoc se3::transform(const se3&) const
	se3 operator*(const se3& x) const;
};

template <class T>
se3<T> se3<T>::inverse() const
{
	const quaternion_type inverse_r = math::conjugate(r);
	const vector_type inverse_t = -(inverse_r * t);
	return se3{inverse_t, inverse_r};
}

template <class T>
typename se3<T>::matrix_type se3<T>::matrix() const
{
	matrix_type m = math::matrix<T, 4, 4>(math::matrix<T, 3, 3>(r));
	
	m[3].x() = t.x();
	m[3].y() = t.y();
	m[3].z() = t.z();
	
	return m;
}

template <class T>
typename se3<T>::vector_type se3<T>::transform(const vector_type& x) const
{
	return r * x + t;
}

template <class T>
se3<T> se3<T>::transform(const se3& x) const
{
	return se3
	{
		x.transform(t),
		math::normalize(x.r * r)
	};
}

template <class T>
typename se3<T>::vector_type se3<T>::operator*(const vector_type& x) const
{
	return transform(x);
}

template <class T>
se3<T> se3<T>::operator*(const se3& x) const
{
	return transform(x);
}

} // namespace transformation
} // namespace math

#endif // ANTKEEPER_MATH_TRANSFORMATION_SE3_HPP

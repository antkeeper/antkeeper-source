/*
 * Copyright (C) 2021  Christopher J. Howard
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

#ifndef ANTKEEPER_PHYSICS_FRAME_HPP
#define ANTKEEPER_PHYSICS_FRAME_HPP

#include "math/math.hpp"
#include <cmath>

namespace physics {

/**
 * 3-dimensional frame of reference.
 *
 * @tparam T Scalar type.
 */
template <class T>
struct frame
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
	
	/// Vector representing a translation from the origin of this frame to the origin of the parent frame.	
	vector_type translation;
	
	/// Quaternion representing a rotation from the orientation of this frame to the orientation of the parent frame.
	quaternion_type rotation;
	
	/// Returns the inverse of this frame.
	frame inverse() const;
	
	/// Returns a transformation matrix representation of this frame.
	matrix_type matrix() const;
	
	/**
	 * Transforms a vector from the parent frame space to this frame's space.
	 *
	 * @param v Vector in parent frame space.
	 * @return Vector in this frame's space.
	 */
	vector_type transform(const vector_type& v) const;
	
	/**
	 * Transforms a frame from the parent frame space to this frame's space.
	 *
	 * @param f Frame in parent frame space.
	 * @return Frame in this frame's space.
	 */
	frame transform(const frame& f) const;
	
	/// @copydoc frame::transform(const vector_type&) const
	vector_type operator*(const vector_type& v) const;
	
	/// @copydoc frame::transform(const frame&) const
	frame operator*(const frame& f) const;
};

template <class T>
frame<T> frame<T>::inverse() const
{
	const quaternion_type inverse_rotation = math::conjugate(rotation);
	const vector_type inverse_translation = -(inverse_rotation * translation);
	return frame{inverse_translation, inverse_rotation};
}

template <class T>
typename frame<T>::matrix_type frame<T>::matrix() const
{
	matrix_type m = math::resize<4, 4>(math::matrix_cast<T>(rotation));
	
	m[3].x = translation.x;
	m[3].y = translation.y;
	m[3].z = translation.z;
	
	return m;
}

template <class T>
typename frame<T>::vector_type frame<T>::transform(const vector_type& v) const
{
	return translation + rotation * v;
}

template <class T>
frame<T> frame<T>::transform(const frame& f) const
{
	return frame
	{
		f.transform(translation),
		math::normalize(f.rotation * rotation)
	};
}

template <class T>
typename frame<T>::vector_type frame<T>::operator*(const vector_type& v) const
{
	return transform(v);
}

template <class T>
frame<T> frame<T>::operator*(const frame& f) const
{
	return transform(f);
}

} // namespace physics

#endif // ANTKEEPER_PHYSICS_FRAME_HPP

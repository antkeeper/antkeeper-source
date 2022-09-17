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

#ifndef ANTKEEPER_MATH_TRANSFORM_TYPE_HPP
#define ANTKEEPER_MATH_TRANSFORM_TYPE_HPP

#include "math/vector-type.hpp"
#include "math/quaternion-type.hpp"

namespace math {

/**
 * Represents 3D TRS transformation.
 *
 * @tparam T Scalar type.
 */
template <class T>
struct transform
{
	/// Translation vector
	vector<T, 3> translation;

	/// Rotation quaternion
	quaternion<T> rotation;

	/// Scale vector
	vector<T, 3> scale;
	
	/// Identity transform.
	static const transform identity;
};

template <class T>
constexpr transform<T> transform<T>::identity =
{
	{T(0), T(0), T(0)},
	{T(1), T(0), T(0), T(0)},
	{T(1), T(1), T(1)}
};

} // namespace math

#endif // ANTKEEPER_MATH_TRANSFORM_TYPE_HPP

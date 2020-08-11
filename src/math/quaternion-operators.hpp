/*
 * Copyright (C) 2020  Christopher J. Howard
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

#ifndef ANTKEEPER_MATH_QUATERNION_OPERATORS_HPP
#define ANTKEEPER_MATH_QUATERNION_OPERATORS_HPP

#include "math/quaternion-type.hpp"
#include "math/quaternion-functions.hpp"

namespace math {
namespace quaternion_operators {

/// @addtogroup quaternion
/// @{

/// @copydoc add(const quaternion<T>&, const quaternion<T>&)
template <class T>
quaternion<T> operator+(const quaternion<T>& x, const quaternion<T>& y);

/// @copydoc div(const quaternion<T>&, T)
template <class T>
quaternion<T> operator/(const quaternion<T>& q, T s);

/// @copydoc mul(const quaternion<T>&, const quaternion<T>&)
template <class T>
quaternion<T> operator*(const quaternion<T>& x, const quaternion<T>& y);

/// @copydoc mul(const quaternion<T>&, T)
template <class T>
quaternion<T> operator*(const quaternion<T>& q, T s);

/// @copydoc mul(const quaternion<T>&, const vector<T, 3>&)
template <class T>
vector<T, 3> operator*(const quaternion<T>& q, const vector<T, 3>& v);

/// @copydoc sub(const quaternion<T>&, const quaternion<T>&)
template <class T>
quaternion<T> operator-(const quaternion<T>& x, const quaternion<T>& y);

/// @copydoc negate(const quaternion<T>&)
template <class T, std::size_t N>
quaternion<T> operator-(const quaternion<T>& x);

template <class T>
inline quaternion<T> operator+(const quaternion<T>& x, const quaternion<T>& y)
{
	return add(x, y);
}

template <class T>
inline quaternion<T> operator/(const quaternion<T>& q, T s)
{
	return div(q, s);
}

template <class T>
inline quaternion<T> operator*(const quaternion<T>& x, const quaternion<T>& y)
{
	return mul(x, y);
}

template <class T>
inline quaternion<T> operator*(const quaternion<T>& q, T s)
{
	return mul(q, s);
}

template <class T>
inline vector<T, 3> operator*(const quaternion<T>& q, const vector<T, 3>& v)
{
	return mul(q, v);
}

template <class T>
inline quaternion<T> operator-(const quaternion<T>& x, const quaternion<T>& y)
{
	return sub(x, y);
}

template <class T, std::size_t N>
inline quaternion<T> operator-(const quaternion<T>& x)
{
	return negate(x);
}

/// @}

} // namespace quaternion_operators
} // namespace math

/// Bring quaternion operators into global namespace
using namespace math::quaternion_operators;

#endif // ANTKEEPER_MATH_QUATERNION_OPERATORS_HPP


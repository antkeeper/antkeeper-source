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

#ifndef ANTKEEPER_MATH_QUATERNION_OPERATORS_HPP
#define ANTKEEPER_MATH_QUATERNION_OPERATORS_HPP

#include "math/quaternion-type.hpp"
#include "math/quaternion-functions.hpp"

/// @copydoc math::add(const math::quaternion<T>&, const math::quaternion<T>&)
template <class T>
math::quaternion<T> operator+(const math::quaternion<T>& x, const math::quaternion<T>& y);

/// @copydoc math::div(const math::quaternion<T>&, T)
template <class T>
math::quaternion<T> operator/(const math::quaternion<T>& q, T s);

/// @copydoc math::mul(const math::quaternion<T>&, const math::quaternion<T>&)
template <class T>
math::quaternion<T> operator*(const math::quaternion<T>& x, const math::quaternion<T>& y);

/// @copydoc math::mul(const math::quaternion<T>&, T)
template <class T>
math::quaternion<T> operator*(const math::quaternion<T>& q, T s);

/// @copydoc math::mul(const math::quaternion<T>&, const math::vector<T, 3>&)
template <class T>
math::vector<T, 3> operator*(const math::quaternion<T>& q, const math::vector<T, 3>& v);

/// @copydoc math::sub(const math::quaternion<T>&, const math::quaternion<T>&)
template <class T>
math::quaternion<T> operator-(const math::quaternion<T>& x, const math::quaternion<T>& y);

/// @copydoc math::negate(const math::quaternion<T>&)
template <class T, std::size_t N>
math::quaternion<T> operator-(const math::quaternion<T>& x);

template <class T>
inline math::quaternion<T> operator+(const math::quaternion<T>& x, const math::quaternion<T>& y)
{
	return add(x, y);
}

template <class T>
inline math::quaternion<T> operator/(const math::quaternion<T>& q, T s)
{
	return div(q, s);
}

template <class T>
inline math::quaternion<T> operator*(const math::quaternion<T>& x, const math::quaternion<T>& y)
{
	return mul(x, y);
}

template <class T>
inline math::quaternion<T> operator*(const math::quaternion<T>& q, T s)
{
	return mul(q, s);
}

template <class T>
inline math::vector<T, 3> operator*(const math::quaternion<T>& q, const math::vector<T, 3>& v)
{
	return mul(q, v);
}

template <class T>
inline math::quaternion<T> operator-(const math::quaternion<T>& x, const math::quaternion<T>& y)
{
	return sub(x, y);
}

template <class T, std::size_t N>
inline math::quaternion<T> operator-(const math::quaternion<T>& x)
{
	return negate(x);
}

#endif // ANTKEEPER_MATH_QUATERNION_OPERATORS_HPP


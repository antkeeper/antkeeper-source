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

#ifndef ANTKEEPER_MATH_TRANSFORM_OPERATORS_HPP
#define ANTKEEPER_MATH_TRANSFORM_OPERATORS_HPP

#include <engine/math/transform-type.hpp>
#include <engine/math/transform-functions.hpp>

/// @copydoc math::mul(const math::transform<T>&, const math::transform<T>&)
template <class T>
[[nodiscard]] math::transform<T> operator*(const math::transform<T>& x, const math::transform<T>& y);

/// @copydoc math::mul(const math::transform<T>&, const math::vector<T, 3>&)
template <class T>
[[nodiscard]] math::vector<T, 3> operator*(const math::transform<T>& t, const math::vector<T, 3>& v);

/**
 * Multiplies two transforms and stores the result in the first transform.
 *
 * @param x First transform.
 * @param y Second transform.
 * @return Reference to the first transform.
 */
template <class T>
math::transform<T>& operator*=(math::transform<T>& x, const math::transform<T>& y);

template <class T>
inline math::transform<T> operator*(const math::transform<T>& x, const math::transform<T>& y)
{
	return mul(x, y);
}

template <class T>
inline math::vector<T, 3> operator*(const math::transform<T>& t, const math::vector<T, 3>& v)
{
	return mul(t, v);
}

template <class T>
inline math::transform<T>& operator*=(math::transform<T>& x, const math::vector<T, 3>& y)
{
	return (x = x * y);
}

#endif // ANTKEEPER_MATH_TRANSFORM_OPERATORS_HPP


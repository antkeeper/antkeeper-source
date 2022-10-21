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

#ifndef ANTKEEPER_COLOR_XYY_HPP
#define ANTKEEPER_COLOR_XYY_HPP

#include "math/vector.hpp"

namespace color {

/// CIE xyY color space.
namespace xyy {

/**
 * Returns the luminance of a CIE xyY color.
 *
 * @param x CIE xyY color.
 * @return return Luminance of @p x.
 */
template <class T>
constexpr inline T luminance(const math::vector3<T>& x)
{
	return x[2];
}

/**
 * Transforms a CIE xyY color into the CIE 1960 UCS colorspace.
 *
 * @param x CIE xyY color.
 * @return CIE 1960 UCS color.
 */
template <class T>
constexpr math::vector2<T> to_ucs(const math::vector3<T>& x)
{
	const T d = T({1} / (T{-2} * x[0] + T{12} * x[1] + T{3}));
	return math::vector2<T>{(T{4} * x[0]) * d, (T{6} * x[1]) * d};
}

/**
 * Transforms a CIE xyY color into the CIE XYZ colorspace.
 *
 * @param x CIE xyY color.
 * @return CIE XYZ color.
 */
template <class T>
constexpr math::vector3<T> to_xyz(const math::vector3<T>& x)
{
	return math::vector3<T>{(x[0] * x[2]) / x[1], x[2], ((T{1} - x[0] - x[1]) * x[2]) / x[1]};
}

} // namespace xyy
} // namespace color

#endif // ANTKEEPER_COLOR_XYY_HPP

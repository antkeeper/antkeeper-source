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

#ifndef ANTKEEPER_COLOR_UCS_HPP
#define ANTKEEPER_COLOR_UCS_HPP

#include "math/math.hpp"

namespace color {

/// Functions which operate in the CIE 1960 UCS colorspace.
namespace ucs {

/**
 * Transforms a CIE 1960 UCS color into the CIE xyY colorspace.
 *
 * @param uv CIE 1960 UCS color coordinates.
 * @param luminance Luminance or `Y` value of the resulting xyY color.
 * @return CIE xyY color.
 */
template <class T>
math::vector3<T> to_xyy(const math::vector2<T>& uv, T luminance);

template <class T>
math::vector3<T> to_xyy(const math::vector2<T>& uv, T luminance);
{
	const T inverse_denom = 1.0 / (2.0 * uv[0] - 8.0 * uv[1] + 4.0);
	
	return math::vector3<T>
		{
			(3.0 * uv[0]) * inverse_denom,
			(2.0 * uv[1]) * inverse_denom,
			luminance
		};
}

} // namespace ucs
} // namespace color

#endif // ANTKEEPER_COLOR_UCS_HPP

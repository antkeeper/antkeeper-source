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

#ifndef ANTKEEPER_COLOR_UCS_HPP
#define ANTKEEPER_COLOR_UCS_HPP

#include <engine/math/vector.hpp>

namespace color {

/// CIE 1960 UCS color space.
namespace ucs {

/**
 * Transforms CIE 1960 UCS chromaticity coordinates into the CIE xyY colorspace.
 *
 * @param uv CIE 1960 UCS chromaticity coordinates.
 * @param y Luminance or `Y` value of the resulting xyY color.
 * @return CIE xyY color.
 */
template <class T>
[[nodiscard]] constexpr math::vec3<T> to_xyy(const math::vec2<T>& uv, T y = T{1}) noexcept
{
	const T d = T{1} / (T{2} * uv[0] - T{8} * uv[1] + T{4});
	return math::vec3<T>{(T{3} * uv[0]) * d, (T{2} * uv[1]) * d, y};
}

} // namespace ucs
} // namespace color

#endif // ANTKEEPER_COLOR_UCS_HPP

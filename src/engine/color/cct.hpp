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

#ifndef ANTKEEPER_COLOR_CCT_HPP
#define ANTKEEPER_COLOR_CCT_HPP

#include <engine/color/ucs.hpp>
#include <engine/color/xyy.hpp>
#include <engine/math/vector.hpp>

namespace color {

/// Correlated color temperature (CCT).
namespace cct {

/**
 * Calculates CIE 1960 UCS colorspace chromaticity coordinates given a correlated color temperature using Krystek's algorithm.
 *
 * @param t Correlated color temperature, in Kelvin.
 * @return CIE 1960 UCS colorspace chromaticity coordinates.
 *
 * @see Krystek, M. (1985), An algorithm to calculate correlated colour temperature. Color Res. Appl., 10: 38-40.
 */
template <class T>
math::vector2<T> to_ucs(T t)
{
	const T tt = t * t;
	return math::vector2<T>
	{
		(T{0.860117757} + T{1.54118254e-4} * t + T{1.28641212e-7} * tt) / (T{1} + T{8.42420235e-4} * t + T{7.08145163e-7} * tt),
		(T{0.317398726} + T{4.22806245e-5} * t + T{4.20481691e-8} * tt) / (T{1} - T{2.89741816e-5} * t + T{1.61456053e-7} * tt)
	};
}

/**
 * Calculates CIE xyY colorspace chromaticity coordinates given a correlated color temperature using Krystek's algorithm.
 *
 * @param t Correlated color temperature, in Kelvin.
 * @return CIE xyY color with `Y = 1`.
 */
template <class T>
math::vector3<T> to_xyy(T t)
{
	return ucs::to_xyy(to_ucs(t), T{1});
}

/**
 * Calculates CIE XYZ colorspace chromaticity coordinates given a correlated color temperature using Krystek's algorithm.
 *
 * @param t Correlated color temperature, in Kelvin.
 * @return CIE XYZ color with `Y = 1`.
 */
template <class T>
math::vector3<T> to_xyz(T t)
{
	return xyy::to_xyz(to_xyy(t));
}

} // namespace cct
} // namespace color

#endif // ANTKEEPER_COLOR_CCT_HPP

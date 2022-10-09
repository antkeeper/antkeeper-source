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

#ifndef ANTKEEPER_COLOR_XYZ_HPP
#define ANTKEEPER_COLOR_XYZ_HPP

#include "math/math.hpp"

namespace color {

/**
 * CIE XYZ color space.
 *
 * @see https://en.wikipedia.org/wiki/CIE_1931_color_space
 */
namespace xyz {

/**
 * Returns the luminance of a CIE XYZ color.
 *
 * @param x CIE XYZ color.
 * @return return Luminance of @p x.
 */
template <class T>
constexpr inline T luminance(const math::vector3<T>& x)
{
	return x[1];
}

/**
 * Transforms a CIE XYZ color into the CIE xyY color space.
 *
 * @param x CIE XYZ color.
 * @return CIE xyY color.
 */
template <class T>
constexpr math::vector3<T> to_xyy(const math::vector3<T>& x)
{
	const T sum = x[0] + x[1] + x[2];
	return math::vector3<T>{x[0] / sum, x[1] / sum, x[1]}
}

/**
 * CIE 1931 standard observer color matching function for the X tristimulus value.
 *
 * @param lambda Wavelength of light, in nanometers.
 * @return Matching X tristimulus value.
 *
 * @see match(T)
 */
template <class T>
T match_x(T lambda)
{
	const T t0 = (lambda - T(442.0)) * ((lambda < T(442.0)) ? T(0.0624) : T(0.0374));
	const T t1 = (lambda - T(599.8)) * ((lambda < T(599.8)) ? T(0.0264) : T(0.0323));
	const T t2 = (lambda - T(501.1)) * ((lambda < T(501.1)) ? T(0.0490) : T(0.0382));
	
	const T x0 = T( 0.362) * std::exp(T(-0.5) * t0 * t0);
	const T x1 = T( 1.056) * std::exp(T(-0.5) * t1 * t1);
	const T x2 = T(-0.065) * std::exp(T(-0.5) * t2 * t2);
	
	return x0 + x1 + x2;
}

/**
 * CIE 1931 standard observer color matching function for the Y tristimulus value.
 *
 * @param lambda Wavelength of light, in nanometers.
 * @return Matching Y tristimulus value.
 *
 * @see match(T)
 */
template <class T>
T match_y(T lambda)
{
	const T t0 = (lambda - T(568.8)) * ((lambda < T(568.8)) ? T(0.0213) : T(0.0247));
	const T t1 = (lambda - T(530.9)) * ((lambda < T(530.9)) ? T(0.0613) : T(0.0322));
	
	const T y0 = T(0.821) * std::exp(T(-0.5) * t0 * t0);
	const T y1 = T(0.286) * std::exp(T(-0.5) * t1 * t1);
	
	return y0 + y1;
}

/**
 * CIE 1931 standard observer color matching function for the Z tristimulus value.
 *
 * @param lambda Wavelength of light, in nanometers.
 * @return Matching Z tristimulus value.
 *
 * @see match(T)
 */
template <class T>
T match_z(T lambda)
{
	const T t0 = (lambda - T(437.0)) * ((lambda < T(437.0)) ? T(0.0845) : T(0.0278));
	const T t1 = (lambda - T(459.0)) * ((lambda < T(459.0)) ? T(0.0385) : T(0.0725));
	
	const T z0 = T(1.217) * std::exp(T(-0.5) * t0 * t0);
	const T z1 = T(0.681) * std::exp(T(-0.5) * t1 * t1);
	
	return z0 + z1;
}

/**
 * Fitted piecewise gaussian approximation to the CIE 1931 standard observer color matching function.
 *
 * @param lambda Wavelength of light, in nanometers.
 * @return Matching CIE XYZ color.
 *
 * @see match_x(T)
 * @see match_y(T)
 * @see match_z(T)
 *
 * @see Wyman, C., Sloan, P.J., & Shirley, P. (2013). Simple Analytic Approximations to the CIE XYZ Color Matching Functions.
 */
template <class T>
math::vector3<T> match(T lambda)
{
	return math::vector3<T>
	{
		match_x<T>(lambda),
		match_y<T>(lambda),
		match_z<T>(lambda)
	};
}

} // namespace xyz
} // namespace color

#endif // ANTKEEPER_COLOR_XYZ_HPP

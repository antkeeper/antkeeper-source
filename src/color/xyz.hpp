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

/// Functions which operate in the CIE XYZ colorspace.
namespace xyz {

/**
 * Returns the luminance of a CIE XYZ color.
 *
 * @param x CIE XYZ color.
 * @return return Luminance of @p x.
 */
template <class T>
T luminance(const math::vector3<T>& x);

/**
 * Transforms a CIE XYZ color into the ACEScg colorspace.
 *
 * @param x CIE XYZ color.
 * @return ACEScg color.
 */
template <class T>
math::vector3<T> to_acescg(const math::vector3<T>& x);

/**
 * Transforms a CIE XYZ color into the linear sRGB colorspace.
 *
 * @param x CIE XYZ color.
 * @return Linear sRGB color.
 */
template <class T>
math::vector3<T> to_srgb(const math::vector3<T>& x);

template <class T>
inline T luminance(const math::vector3<T>& x)
{
	return x[1];
}

template <class T>
math::vector3<T> to_acescg(const math::vector3<T>& x)
{
	static const math::matrix3<T> xyz_to_acescg
	{{
		{ 1.641023379694326 -0.663662858722983  0.011721894328375},
		{-0.324803294184790  1.615331591657338 -0.008284441996237},
		{-0.236424695237612, 0.016756347685530, 0.988394858539022}
	}};
	
	return xyz_to_acescg * x;
}

template <class T>
math::vector3<T> to_srgb(const math::vector3<T>& x)
{
	static const math::matrix3<T> xyz_to_srgb
	{{
		{ 3.240969941904523, -0.969243636280880,  0.055630079696994},
		{-1.537383177570094,  1.875967501507721, -0.203976958888977},
		{-0.498610760293003,  0.041555057407176,  1.056971514242879}
	}};
	
	return xyz_to_srgb * x;
}

} // namespace xyz
} // namespace color

#endif // ANTKEEPER_COLOR_XYZ_HPP

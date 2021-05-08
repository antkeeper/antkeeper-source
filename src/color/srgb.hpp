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

#ifndef ANTKEEPER_COLOR_SRGB_HPP
#define ANTKEEPER_COLOR_SRGB_HPP

#include "math/math.hpp"
#include <cmath>

namespace color {

/// Functions which operate in the sRGB colorspace.
namespace srgb {

/**
 * Performs the sRGB Electro-Optical Transfer Function (EOTF), also known as the sRGB decoding function.
 *
 * @param v sRGB electrical signal (gamma-encoded sRGB).
 * @return Corresponding luminance of the signal (linear sRGB).
 */
/// @{
float eotf(float v);
double eotf(double v);
template <class T>
math::vector3<T> eotf(const math::vector3<T>& v);
/// @}

/**
 * Performs the sRGB inverse Electro-Optical Transfer Function (EOTF), also known as the sRGB encoding function.
 *
 * @param l sRGB luminance (linear sRGB).
 * @return Corresponding electrical signal (gamma-encoded sRGB).
 */
/// @{
float eotf_inverse(float v);
double eotf_inverse(double v);
template <class T>
math::vector3<T> eotf_inverse(const math::vector3<T>& l);
/// @}

/**
 * Calculates the luminance of a linear sRGB color.
 *
 * @param x Linear sRGB color.
 * @return return Luminance of @p x.
 */
template <class T>
T luminance(const math::vector3<T>& x);

/**
 * Transforms a linear sRGB color into the ACEScg colorspace using the Bradford chromatic adaption transform.
 *
 * @param x Linear sRGB color.
 * @return ACEScg color.
 *
 * @see https://www.colour-science.org/apps/
 */
template <class T>
math::vector3<T> to_acescg(const math::vector3<T>& x);

/**
 * Transforms a linear sRGB color into the CIE XYZ colorspace.
 *
 * @param x Linear sRGB color.
 * @return CIE XYZ color.
 */
template <class T>
math::vector3<T> to_xyz(const math::vector3<T>& x);

inline float eotf(float v)
{
	return (v < 0.04045f) ? (v / 12.92f) : std::pow((v + 0.055f) / 1.055f, 2.4f);
}

inline double eotf(double v)
{
	return (v < 0.04045) ? (v / 12.92) : std::pow((v + 0.055) / 1.055, 2.4);
}

template <class T>
math::vector3<T> eotf(const math::vector3<T>& v)
{
	return math::vector3<T>
		{
			eotf(v[0]),
			eotf(v[1]),
			eotf(v[2])
		};
}

inline float eotf_inverse(float l)
{
	return (l <= 0.0031308f) ? (l * 12.92f) : (std::pow(l, 1.0f / 2.4f) * 1.055f - 0.055f);
}

inline double eotf_inverse(double l)
{
	return (l <= 0.0031308) ? (l * 12.92) : (std::pow(l, 1.0 / 2.4) * 1.055 - 0.055);
}

template <class T>
math::vector3<T> eotf_inverse(const math::vector3<T>& l)
{
	return math::vector3<T>
		{
			eotf_inverse(l[0]),
			eotf_inverse(l[1]),
			eotf_inverse(l[2])
		};
}

template <class T>
T luminance(const math::vector3<T>& x)
{
	static const math::vector3<T> luma = {0.212639005871510, 0.715168678767756, 0.072192315360734};
	return math::dot(x, luma);
}

template <class T>
math::vector3<T> to_acescg(const math::vector3<T>& x)
{
	static const math::matrix3<T> srgb_to_acescg
	{{
		{0.613132422390542, 0.070124380833917, 0.020587657528185},
		{0.339538015799666, 0.916394011313573, 0.109574571610682},
		{0.047416696048269, 0.013451523958235, 0.869785404035327}
	}};
	
	return srgb_to_acescg * x;
}

template <class T>
math::vector3<T> to_xyz(const math::vector3<T>& x)
{
	static const math::matrix3<T> srgb_to_xyz
	{{
		{0.412390799265959, 0.212639005871510, 0.019330818715592},
		{0.357584339383878, 0.715168678767756, 0.119194779794626},
		{0.180480788401834, 0.072192315360734, 0.950532152249661}
	}};
	
	return srgb_to_xyz * x;
}

} // namespace srgb
} // namespace color

#endif // ANTKEEPER_COLOR_SRGB_HPP

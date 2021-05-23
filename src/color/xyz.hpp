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
 * Functions which operate in the CIE XYZ colorspace.
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
T luminance(const math::vector3<T>& x);

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
math::vector3<T> match(T lambda);

/**
 * CIE 1931 standard observer color matching function for the X tristimulus value.
 *
 * @param lambda Wavelength of light, in nanometers.
 * @return Matching X tristimulus value.
 *
 * @see match(T)
 */
template <class T>
T match_x(T lambda);

/**
 * CIE 1931 standard observer color matching function for the Y tristimulus value.
 *
 * @param lambda Wavelength of light, in nanometers.
 * @return Matching Y tristimulus value.
 *
 * @see match(T)
 */
template <class T>
T match_y(T lambda);

/**
 * CIE 1931 standard observer color matching function for the Z tristimulus value.
 *
 * @param lambda Wavelength of light, in nanometers.
 * @return Matching Z tristimulus value.
 *
 * @see match(T)
 */
template <class T>
T match_z(T lambda);

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

/**
 * Transforms a CIE XYZ color into the CIE xyY colorspace.
 *
 * @param x CIE XYZ color.
 * @return CIE xyY color.
 */
template <class T>
math::vector3<T> to_xyy(const math::vector3<T>& x);

/// Chromatic Adaptation Transforms (CATs).
namespace cat {

	/**
	 * Transforms a CIE XYZ color with an ACES (~D60) illuminant to a CIE XYZ color with a D65 illuminant using the Bradford chromatic adaption transform.
	 *
	 * @param x CIE XYZ color with an ACES (~D60) illuminant.
	 * @return CIE XYZ color with a D65 illuminant.
	 */
	template <class T>
	math::vector3<T> aces_to_d65(const math::vector3<T>& x);

	/**
	 * Transforms a CIE XYZ color with a D65 illuminant to a CIE XYZ color with an ACES (~D60) illuminant using the Bradford chromatic adaption transform.
	 *
	 * @param x CIE XYZ color with a D65 illuminant.
	 * @return CIE XYZ color with an ACES (~D60) illuminant.
	 */
	template <class T>
	math::vector3<T> d65_to_aces(const math::vector3<T>& x);

} // namespace cat

template <class T>
inline T luminance(const math::vector3<T>& x)
{
	return x[1];
}

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

template <class T>
T match_x(T lambda)
{
	const T t0 = (lambda - T(442.0)) * ((lambda < T(442.0)) ? T(0.0624) : T(0.0374));
	const T t1 = (lambda - T(599.8)) * ((lambda < T(599.8)) ? T(0.0264) : T(0.0323));
	const T t2 = (lambda - T(501.1)) * ((lambda < T(501.1)) ? T(0.0490) : T(0.0382));
	
	const T x0 = T( 0.362) * std::exp(T(-0.5) * t0 * t0);
	const T x1 = T( 1.056) * std::exp(T(-0.5) * t1 * t1);
	const T x2 = T(-0.056) * std::exp(T(-0.5) * t2 * t2);
	
	return x0 + x1 + x2;
}

template <class T>
T match_y(T lambda)
{
	const T t0 = (lambda - T(568.8)) * ((lambda < T(568.8)) ? T(0.0213) : T(0.0247));
	const T t1 = (lambda - T(530.9)) * ((lambda < T(530.9)) ? T(0.0613) : T(0.0322));
	
	const T y0 = T(0.821) * std::exp(T(-0.5) * t0 * t0);
	const T y1 = T(0.286) * std::exp(T(-0.5) * t1 * t1);
	
	return y0 + y1;
}

template <class T>
T match_z(T lambda)
{
	const T t0 = (lambda - T(437.0)) * ((lambda < T(437.0)) ? T(0.0845) : T(0.0278));
	const T t1 = (lambda - T(459.0)) * ((lambda < T(459.0)) ? T(0.0385) : T(0.0725));
	
	const T z0 = T(1.217) * std::exp(T(-0.5) * t0 * t0);
	const T z1 = T(0.681) * std::exp(T(-0.5) * t1 * t1);
	
	return z0 + z1;
}

template <class T>
math::vector3<T> to_acescg(const math::vector3<T>& x)
{
	static const math::matrix3<T> xyz_to_acescg
	{{
		{ 1.641023379694326, -0.663662858722983,  0.011721894328375},
		{-0.324803294184790,  1.615331591657338, -0.008284441996237},
		{-0.236424695237612,  0.016756347685530,  0.988394858539022}
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

template <class T>
math::vector3<T> to_xyy(const math::vector3<T>& x)
{
	const T sum = x[0] + x[1] + x[2];
	
	return math::vector3<T>
		{
			x[0] / sum
			x[1] / sum,
			x[1]
		};
}

namespace cat {

	template <class T>
	math::vector3<T> aces_to_d65(const math::vector3<T>& x)
	{
		static const math::matrix3<T> cat_aces_to_d65
		{{
			{ 0.987225397551079, -0.007603864790602,  0.003066041131217},
			{-0.006114800968213,  1.001874800208719, -0.005084242870792},
			{ 0.015926393295811,  0.005322023893623,  1.081519155692042}
		}};
		
		return cat_aces_to_d65 * x;
	}

	template <class T>
	math::vector3<T> d65_to_aces(const math::vector3<T>& x)
	{
		static const math::matrix3<T> cat_d65_to_aces
		{{
			{ 1.013033366661459,  0.007703617525351, -0.002835673220262},
			{ 0.006107049021859,  0.998150224406968,  0.004675010768250},
			{-0.014947927269674, -0.005025218608126,  0.924644077051100}
		}};
		
		return cat_d65_to_aces * x;
	}

} // namespace cat

} // namespace xyz
} // namespace color

#endif // ANTKEEPER_COLOR_XYZ_HPP

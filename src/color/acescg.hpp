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

#ifndef ANTKEEPER_COLOR_ACESCG_HPP
#define ANTKEEPER_COLOR_ACESCG_HPP

#include "math/math.hpp"

namespace color {

/// Functions which operate in the ACEScg colorspace.
namespace acescg {

/**
 * Calculates the luminance of an ACEScg color.
 *
 * @param x ACEScg color.
 * @return return Luminance of @p x.
 */
template <class T>
T luminance(const math::vector3<T>& x);

/**
 * Transforms an ACEScg color into the linear sRGB colorspace using the Bradford chromatic adaption transform.
 *
 * @param x ACEScg color.
 * @return return Linear sRGB color.
 *
 * @see https://www.colour-science.org/apps/
 */
template <class T>
math::vector3<T> to_srgb(const math::vector3<T>& x);

/**
 * Transforms an ACEScg color into the CIE XYZ colorspace.
 *
 * @param x ACEScg color.
 * @return return CIE XYZ color.
 */
template <class T>
math::vector3<T> to_xyz(const math::vector3<T>& x);

template <class T>
T luminance(const math::vector3<T>& x)
{
	static const math::vector3<T> luma = {0.272228716780914, 0.674081765811148, 0.053689517407937};
	return math::dot(x, luma);
}

template <class T>
math::vector3<T> to_srgb(const math::vector3<T>& x)
{
	static const math::matrix3<T> acescg_to_srgb
	{{
		{ 1.704858676289160, -0.130076824208823, -0.023964072927574},
		{-0.621716021885330,  1.140735774822504, -0.128975508299318},
		{-0.083299371729058, -0.010559801677511,  1.153014018916862}
	}};
	
	return acescg_to_srgb * x;
}

template <class T>
math::vector3<T> to_xyz(const math::vector3<T>& x)
{
	static const math::matrix3<T> acescg_to_xyz
	{{
		{0.662454181108505, 0.272228716780914, -0.005574649490394},
		{0.134004206456433, 0.674081765811148,  0.004060733528983},
		{0.156187687004908, 0.053689517407937,  1.010339100312997}
	}};
	
	return acescg_to_xyz * x;
}

} // namespace acescg
} // namespace color

#endif // ANTKEEPER_COLOR_ACESCG_HPP

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

#ifndef ANTKEEPER_ACES_HPP
#define ANTKEEPER_ACES_HPP

#include "math/vector-type.hpp"

/**
 * Transforms a linear sRGB color into the ACEScg colorspace, using the Bradford chromatic adaption transform.
 *
 * @param x Linear sRGB color.
 * @return Linear ACEScg color.
 *
 * @see https://www.colour-science.org/apps/
 */
template <class T>
math::vector<T, 3> srgb_to_acescg(const math::vector<T, 3>& x)
{
	static const math::matrix<T, 3, 3> matrix
	{{
		{0.6131324224, 0.0701243808, 0.0205876575},
		{0.3395380158, 0.9163940113, 0.1095745716},
		{0.0474166960, 0.0134515240, 0.8697854040}
	}};
	
	return matrix * x;
}

/**
 * Transforms an ACEScg color into the linear sRGB colorspace, using the Bradford chromatic adaption transform.
 *
 * @param x Linear ACEScg color.
 * @param return Linear sRGB color.
 *
 * @see https://www.colour-science.org/apps/
 */
template <class T>
math::vector<T, 3> acescg_to_srgb(const math::vector<T, 3>& x)
{
	static const math::matrix<T, 3, 3> matrix
	{{
		{ 1.7048586763, -0.1300768242, -0.0239640729},
		{-0.6217160219,  1.1407357748, -0.1289755083},
		{-0.0832993717, -0.0105598017,  1.1530140189}
	}};
	
	return matrix * x;
}

/**
 * Calculates the luminance of an ACEScg color.
 *
 * @param x Linear ACEScg color.
 * @param return Luminance of @p x.
 */
template <class T>
T acescg_to_luminance(const math::vector<T, 3>& x)
{
	static const math::vector<T, 3> luma = {0.2722287168, 0.6740817658, 0.0536895174};
	return math::dot(x, luma);
}

#endif // ANTKEEPER_ACES_HPP

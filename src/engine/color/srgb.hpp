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

#ifndef ANTKEEPER_COLOR_SRGB_HPP
#define ANTKEEPER_COLOR_SRGB_HPP

#include <engine/color/rgb.hpp>
#include <engine/color/illuminant.hpp>
#include <engine/math/vector.hpp>
#include <cmath>

namespace color {

/**
 * sRGB electro-optical transfer function (EOTF), also known as the sRGB decoding function.
 *
 * @param v sRGB electrical signal (gamma-encoded sRGB).
 *
 * @return Corresponding luminance of the signal (linear sRGB).
 */
template <class T>
math::vector3<T> srgb_eotf(const math::vector3<T>& v)
{
	auto f = [](T x) -> T
	{
		return x < T{0.04045} ? x / T{12.92} : std::pow((x + T{0.055}) / T{1.055}, T{2.4});
	};
	
	return math::vector3<T>
	{
		f(v[0]),
		f(v[1]),
		f(v[2])
	};
}

/**
 * sRGB inverse electro-optical transfer function (EOTF), also known as the sRGB encoding function.
 *
 * @param l sRGB luminance (linear sRGB).
 *
 * @return Corresponding electrical signal (gamma-encoded sRGB).
 */
template <class T>
math::vector3<T> srgb_inverse_eotf(const math::vector3<T>& l)
{
	auto f = [](T x) -> T
	{
		return x <= T{0.0031308} ? x * T{12.92} : std::pow(x, T{1} / T{2.4}) * T{1.055} - T{0.055};
	};
	
	return math::vector3<T>
	{
		f(l[0]),
		f(l[1]),
		f(l[2])
	};
}

/// sRGB color space.
template <class T>
constexpr rgb::color_space<T> srgb
(
	{T{0.64}, T{0.33}},
	{T{0.30}, T{0.60}},
	{T{0.15}, T{0.06}},
	color::illuminant::deg2::d65<T>,
	&srgb_eotf<T>,
	&srgb_inverse_eotf
);

} // namespace color

#endif // ANTKEEPER_COLOR_SRGB_HPP

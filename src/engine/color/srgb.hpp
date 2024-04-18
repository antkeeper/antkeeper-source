// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_COLOR_SRGB_HPP
#define ANTKEEPER_COLOR_SRGB_HPP

#include <engine/color/rgb.hpp>
#include <cmath>

namespace color {

/// @name sRGB color space
/// @{

/**
 * sRGB Electro-Optical Transfer Function (EOTF).
 *
 * @param x Non-linear sRGB signal.
 *
 * @return Linear sRGB tristimulus.
 */
template <class T>
[[nodiscard]] math::vec3<T> srgb_eotf(const math::vec3<T>& x)
{
	auto f = [](T x) -> T
	{
		return x <= T{12.92} * T{0.0031308} ? x / T{12.92} : std::pow((x + T{0.055}) / T{1.055}, T{2.4});
	};
	
	return {f(x[0]), f(x[1]), f(x[2])};
}

/**
 * sRGB inverse EOTF.
 *
 * @param x Linear sRGB tristimulus.
 *
 * @return Non-linear sRGB signal.
 */
template <class T>
[[nodiscard]] math::vec3<T> srgb_inverse_eotf(const math::vec3<T>& x)
{
	auto f = [](T x) -> T
	{
		return x <= T{0.0031308} ? T{12.92} * x : T{1.055} * std::pow(x, T{1.0 / 2.4}) - T{0.055};
	};
	
	return {f(x[0]), f(x[1]), f(x[2])};
}

/**
 * sRGB color space.
 *
 * @see IEC 61966-2-1:1999
 */
template <class T>
constexpr rgb_color_space<T> srgb
(
	{T{0.6400}, T{0.3300}},
	{T{0.3000}, T{0.6000}},
	{T{0.1500}, T{0.0600}},
	{T{0.3127}, T{0.3290}},
	&srgb_inverse_eotf<T>,
	&srgb_eotf<T>
);

/// @}

} // namespace color

#endif // ANTKEEPER_COLOR_SRGB_HPP

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

#ifndef ANTKEEPER_COLOR_ACES_HPP
#define ANTKEEPER_COLOR_ACES_HPP

#include <engine/color/rgb.hpp>
#include <engine/math/matrix.hpp>
#include <engine/math/vector.hpp>

namespace color {

/// ACES color spaces.
namespace aces {

/// CIE xy chromaticity coordinates of the ACES white point (~D60).
template <class T>
constexpr math::vector2<T> white_point = {T{0.32168}, T{0.33767}};

/// ACES AP0 color space.
template <class T>
constexpr rgb::color_space<T> ap0
(
	{T{0.7347}, T{ 0.2653}},
	{T{0.0000}, T{ 1.0000}},
	{T{0.0001}, T{-0.0770}},
	aces::white_point<T>,
	nullptr,
	nullptr
);

/// ACES AP1 color space.
template <class T>
constexpr rgb::color_space<T> ap1
(
	{T{0.713}, T{0.293}},
	{T{0.165}, T{0.830}},
	{T{0.128}, T{0.044}},
	aces::white_point<T>,
	nullptr,
	nullptr
);

/**
 * Constructs a saturation adjustment matrix.
 *
 * @param s Saturation adjustment factor.
 * @param to_y Color space to CIE XYZ luminance vector.
 *
 * @return Saturation adjustment matrix.
 */
template <class T>
[[nodiscard]] constexpr math::matrix<T, 3, 3> adjust_saturation(T s, const math::vector3<T>& to_y) noexcept
{
	const math::vector3<T> v = to_y * (T{1} - s);
	return math::matrix<T, 3, 3>
	{
		v[0] + s, v[0],     v[0],
		v[1],     v[1] + s, v[1],
		v[2],     v[2],     v[2] + s
	};
}

/// ACES AP1 RRT saturation adjustment matrix.
template <class T>
constexpr math::matrix<T, 3, 3> ap1_rrt_sat = aces::adjust_saturation(T{0.96}, ap1<T>.to_y);

/// ACES AP1 ODT saturation adjustment matrix.
template <class T>
constexpr math::matrix<T, 3, 3> ap1_odt_sat = aces::adjust_saturation(T{0.93}, ap1<T>.to_y);

} // namespace aces

} // namespace color

#endif // ANTKEEPER_COLOR_ACES_HPP

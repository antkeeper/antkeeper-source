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

#ifndef ANTKEEPER_MATH_PROJECTION_HPP
#define ANTKEEPER_MATH_PROJECTION_HPP

#include <cmath>

namespace math {

/**
 * Calculates a horizontal FoV given a vertical FoV and aspect ratio.
 *
 * @param v Vertical FoV, in radians.
 * @param r Ratio of width to height.
 *
 * @return Horizontal FoV, in radians.
 *
 * @see https://en.wikipedia.org/wiki/Field_of_view_in_video_games
 */
template <class T>
T horizontal_fov(T v, T r)
{
	return T{2} * std::atan(std::tan(v * T{0.5}) * r);
}

/**
 * Calculates a vertical FoV given a horizontal FoV and aspect ratio.
 *
 * @param h Horizontal FoV, in radians.
 * @param r Ratio of width to height.
 *
 * @return Vertical FoV, in radians.
 *
 * @see https://en.wikipedia.org/wiki/Field_of_view_in_video_games
 */
template <class T>
T vertical_fov(T h, T r)
{
	return T{2} * std::atan(std::tan(h * T{0.5}) / r);
}

} // namespace math

#endif // ANTKEEPER_MATH_PROJECTION_HPP

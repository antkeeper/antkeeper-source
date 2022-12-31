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

#ifndef ANTKEEPER_MATH_PROJECTION_HPP
#define ANTKEEPER_MATH_PROJECTION_HPP

#include "math/matrix.hpp"
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

/**
 * Creates an orthographic projection matrix which will transform the near and far clipping planes to `[-1, 1]`, respectively.
 *
 * @param left Signed distance to the left clipping plane.
 * @param right Signed distance to the right clipping plane.
 * @param bottom Signed distance to the bottom clipping plane.
 * @param top Signed distance to the top clipping plane.
 * @param z_near Signed distance to the near clipping plane.
 * @param z_far Signed distance to the far clipping plane.
 *
 * @return Orthographic projection matrix.
 */
template <class T>
constexpr matrix<T, 4, 4> ortho(T left, T right, T bottom, T top, T z_near, T z_far) noexcept
{
	return
		{{
			 {T(2) / (right - left), T(0), T(0), T(0)},
			 {T(0), T(2) / (top - bottom), T(0), T(0)},
			 {T(0), T(0), T(-2) / (z_far - z_near), T(0)},
			 {-((right + left) / (right - left)), -((top + bottom) / (top - bottom)), -((z_far + z_near) / (z_far - z_near)), T(1)}
		}};
}

/**
 * Creates an orthographic projection matrix which will transform the near and far clipping planes to `[0, 1]`, respectively.
 *
 * @param left Signed distance to the left clipping plane.
 * @param right Signed distance to the right clipping plane.
 * @param bottom Signed distance to the bottom clipping plane.
 * @param top Signed distance to the top clipping plane.
 * @param z_near Signed distance to the near clipping plane.
 * @param z_far Signed distance to the far clipping plane.
 *
 * @return Orthographic projection matrix.
 */
template <class T>
constexpr matrix<T, 4, 4> ortho_half_z(T left, T right, T bottom, T top, T z_near, T z_far) noexcept
{
	return
		{{
			 {T(2) / (right - left), T(0), T(0), T(0)},
			 {T(0), T(2) / (top - bottom), T(0), T(0)},
			 {T(0), T(0), T(-1) / (z_far - z_near), T(0)},
			 {-((right + left) / (right - left)), -((top + bottom) / (top - bottom)), -z_near / (z_far - z_near), T(1)}
		}};
}

/**
 * Creates a perspective projection matrix which will transform the near and far clipping planes to `[-1, 1]`, respectively.
 *
 * @param vertical_fov Vertical field of view angle, in radians.
 * @param aspect_ratio Aspect ratio which determines the horizontal field of view.
 * @param z_near Distance to the near clipping plane.
 * @param z_far Distance to the far clipping plane.
 *
 * @return Perspective projection matrix.
 */
template <class T>
matrix<T, 4, 4> perspective(T vertical_fov, T aspect_ratio, T z_near, T z_far)
{
	T half_fov = vertical_fov * T(0.5);
	T f = std::cos(half_fov) / std::sin(half_fov);

	return
		{{
			 {f / aspect_ratio, T(0), T(0), T(0)},
			 {T(0), f, T(0), T(0)},
			 {T(0), T(0), (z_far + z_near) / (z_near - z_far), T(-1)},
			 {T(0), T(0), (T(2) * z_far * z_near) / (z_near - z_far), T(0)}
		}};
}

/**
 * Creates a perspective projection matrix which will transform the near and far clipping planes to `[0, 1]`, respectively.
 *
 * @param vertical_fov Vertical field of view angle, in radians.
 * @param aspect_ratio Aspect ratio which determines the horizontal field of view.
 * @param z_near Distance to the near clipping plane.
 * @param z_far Distance to the far clipping plane.
 *
 * @return Perspective projection matrix.
 */
template <class T>
matrix<T, 4, 4> perspective_half_z(T vertical_fov, T aspect_ratio, T z_near, T z_far)
{
	T half_fov = vertical_fov * T(0.5);
	T f = std::cos(half_fov) / std::sin(half_fov);

	return
		{{
			 {f / aspect_ratio, T(0), T(0), T(0)},
			 {T(0), f, T(0), T(0)},
			 {T(0), T(0), z_far / (z_near - z_far), T(-1)},
			 {T(0), T(0), -(z_far * z_near) / (z_far - z_near), T(0)}
		}};
}

} // namespace math

#endif // ANTKEEPER_MATH_PROJECTION_HPP

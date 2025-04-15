// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.math.projection;
import engine.math.functions;
import engine.math.constants;
import engine.math.matrix;
import <tuple>;

export namespace engine::math
{
	/// Calculates a horizontal FoV given a vertical FoV and aspect ratio.
	/// @param v Vertical FoV, in radians.
	/// @param r Ratio of width to height.
	/// @return Horizontal FoV, in radians.
	/// @see https://en.wikipedia.org/wiki/Field_of_view_in_video_games
	template <class T>
	[[nodiscard]] T horizontal_fov(T v, T r)
	{
		return T{2} *atan(tan(v * T{0.5}) * r);
	}

	/// Calculates a vertical FoV given a horizontal FoV and aspect ratio.
	/// @param h Horizontal FoV, in radians.
	/// @param r Ratio of width to height.
	/// @return Vertical FoV, in radians.
	/// @see https://en.wikipedia.org/wiki/Field_of_view_in_video_games
	template <class T>
	[[nodiscard]] T vertical_fov(T h, T r)
	{
		return T{2} *atan(tan(h * T{0.5}) / r);
	}

	/// Constructs an orthographic projection matrix which will transform the near and far clipping planes to `[-1, 1]`, respectively.
	/// @param left Signed distance to the left clipping plane.
	/// @param right Signed distance to the right clipping plane.
	/// @param bottom Signed distance to the bottom clipping plane.
	/// @param top Signed distance to the top clipping plane.
	/// @param near Signed distance to the near clipping plane.
	/// @param far Signed distance to the far clipping plane.
	/// @return Orthographic projection matrix.
	template <class T>
	[[nodiscard]] constexpr mat4<T> ortho(T left, T right, T bottom, T top, T near, T far) noexcept
	{
		return
		{{
			{T{2} / (right - left), T{0}, T{0}, T{0}},
			{T{0}, T{2} / (top - bottom), T{0}, T{0}},
			{T{0}, T{0}, T{-2} / (far - near), T{0}},
			{-((right + left) / (right - left)), -((top + bottom) / (top - bottom)), -((far + near) / (far - near)), T{1}}
		}};
	}

	/// Constructs an orthographic projection matrix which will transform the near and far clipping planes to `[-1, 1]`, respectively, along with its inverse.
	/// @param left Signed distance to the left clipping plane.
	/// @param right Signed distance to the right clipping plane.
	/// @param bottom Signed distance to the bottom clipping plane.
	/// @param top Signed distance to the top clipping plane.
	/// @param near Signed distance to the near clipping plane.
	/// @param far Signed distance to the far clipping plane.
	/// @return Tuple containing the orthographic projection matrix, followed by its inverse.
	/// @note Constructing the inverse orthographic projection matrix from projection parameters is faster and more precise than inverting matrix.
	template <class T>
	[[nodiscard]] constexpr std::tuple<mat4<T>, mat4<T>> ortho_inv(T left, T right, T bottom, T top, T near, T far) noexcept
	{
		return
		{
			mat4<T>
			{{
				{T{2} / (right - left), T{0}, T{0}, T{0}},
				{T{0}, T{2} / (top - bottom), T{0}, T{0}},
				{T{0}, T{0}, T{-2} / (far - near), T{0}},
				{-((right + left) / (right - left)), -((top + bottom) / (top - bottom)), -((far + near) / (far - near)), T{1}}
			}},

			mat4<T>
			{{
				{(right - left) / T{2}, T{0}, T{0}, T{0}},
				{T{0}, (top - bottom) / T{2}, T{0}, T{0}},
				{T{0}, T{0}, (-far + near) / T{2}, T{0}},
				{(right + left) / T{2}, (bottom + top) / T{2}, (-far - near) / T{2}, T{1}}
			}}
		};
	}

	/// Constructs an orthographic projection matrix which will transform the near and far clipping planes to `[0, 1]`, respectively.
	/// @param left Signed distance to the left clipping plane.
	/// @param right Signed distance to the right clipping plane.
	/// @param bottom Signed distance to the bottom clipping plane.
	/// @param top Signed distance to the top clipping plane.
	/// @param near Signed distance to the near clipping plane.
	/// @param far Signed distance to the far clipping plane.
	/// @return Orthographic projection matrix.
	template <class T>
	[[nodiscard]] constexpr mat4<T> ortho_half_z(T left, T right, T bottom, T top, T near, T far) noexcept
	{
		return
		{{
			{T{2} / (right - left), T{0}, T{0}, T{0}},
			{T{0}, T{2} / (top - bottom), T{0}, T{0}},
			{T{0}, T{0}, T{-1} / (far - near), T{0}},
			{-((right + left) / (right - left)), -((top + bottom) / (top - bottom)), -near / (far - near), T{1}}
		}};
	}

	/// Constructs an orthographic projection matrix which will transform the near and far clipping planes to `[0, 1]`, respectively, along with its inverse.
	/// @param left Signed distance to the left clipping plane.
	/// @param right Signed distance to the right clipping plane.
	/// @param bottom Signed distance to the bottom clipping plane.
	/// @param top Signed distance to the top clipping plane.
	/// @param near Signed distance to the near clipping plane.
	/// @param far Signed distance to the far clipping plane.
	/// @return Tuple containing the orthographic projection matrix, followed by its inverse.
	/// @note Constructing the inverse orthographic projection matrix from projection parameters is faster and more precise than inverting matrix.
	template <class T>
	[[nodiscard]] constexpr std::tuple<mat4<T>, mat4<T>> ortho_half_z_inv(T left, T right, T bottom, T top, T near, T far) noexcept
	{
		return
		{
			mat4<T>
			{{
				{T{2} / (right - left), T{0}, T{0}, T{0}},
				{T{0}, T{2} / (top - bottom), T{0}, T{0}},
				{T{0}, T{0}, T{-1} / (far - near), T{0}},
				{-((right + left) / (right - left)), -((top + bottom) / (top - bottom)), -near / (far - near), T{1}}
			}},

			mat4<T>
			{{
				{(right - left) / T{2}, T{0}, T{0}, T{0}},
				{T{0}, (top - bottom) / T{2}, T{0}, T{0}},
				{T{0}, T{0}, -far + near, T{0}},
				{(right + left) / T{2}, (bottom + top) / T{2}, -near, T{1}}
			}}
		};
	}

	/// Constructs a perspective projection matrix which will transform the near and far clipping planes to `[-1, 1]`, respectively.
	/// @param vertical_fov Vertical field of view angle, in radians.
	/// @param aspect_ratio Aspect ratio which determines the horizontal field of view.
	/// @param near Distance to the near clipping plane.
	/// @param far Distance to the far clipping plane.
	/// @return Perspective projection matrix.
	template <class T>
	[[nodiscard]] mat4<T> perspective(T vertical_fov, T aspect_ratio, T near, T far)
	{
		const T half_fov = vertical_fov * T{0.5};
		const T f = cos(half_fov) / sin(half_fov);

		return
		{{
			{f / aspect_ratio, T{0}, T{0}, T{0}},
			{T{0}, f, T{0}, T{0}},
			{T{0}, T{0}, (far + near) / (near - far), T{-1}},
			{T{0}, T{0}, (T{2} *far * near) / (near - far), T{0}}
		}};
	}

	/// Constructs a perspective projection matrix which will transform the near and far clipping planes to `[-1, 1]`, respectively, along with its inverse.
	/// @param vertical_fov Vertical field of view angle, in radians.
	/// @param aspect_ratio Aspect ratio which determines the horizontal field of view.
	/// @param near Distance to the near clipping plane.
	/// @param far Distance to the far clipping plane.
	/// @return Array containing the perspective projection matrix, followed by its inverse.
	/// @note Constructing the inverse perspective projection matrix from projection parameters is faster and more precise than inverting matrix.
	template <class T>
	[[nodiscard]] std::tuple<mat4<T>, mat4<T>> perspective_inv(T vertical_fov, T aspect_ratio, T near, T far)
	{
		const T half_fov = vertical_fov * T{0.5};
		const T f = cos(half_fov) / sin(half_fov);

		return
		{
			mat4<T>
			{{
				{f / aspect_ratio, T{0}, T{0}, T{0}},
				{T{0}, f, T{0}, T{0}},
				{T{0}, T{0}, (far + near) / (near - far), T{-1}},
				{T{0}, T{0}, (T{2} *far * near) / (near - far), T{0}}
			}},

			mat4<T>
			{{
				{aspect_ratio / f, T{0}, T{0}, T{0}},
				{T{0}, T{1} / f, T{0}, T{0}},
				{T{0}, T{0}, T{0}, (near - far) / (T{2} *near * far)},
				{T{0}, T{0}, T{-1}, (near + far) / (T{2} *near * far)}
			}}
		};
	}

	/// Constructs a perspective projection matrix which will transform the near and far clipping planes to `[0, 1]`, respectively.
	/// @param vertical_fov Vertical field of view angle, in radians.
	/// @param aspect_ratio Aspect ratio which determines the horizontal field of view.
	/// @param near Distance to the near clipping plane.
	/// @param far Distance to the far clipping plane.
	/// @return Perspective projection matrix.
	template <class T>
	[[nodiscard]] mat4<T> perspective_half_z(T vertical_fov, T aspect_ratio, T near, T far)
	{
		const T half_fov = vertical_fov * T{0.5};
		const T f = cos(half_fov) / sin(half_fov);

		return
		{{
			{f / aspect_ratio, T{0}, T{0}, T{0}},
			{T{0}, f, T{0}, T{0}},
			{T{0}, T{0}, far / (near - far), T{-1}},
			{T{0}, T{0}, -(far * near) / (far - near), T{0}}
		}};
	}

	/// Constructs a perspective projection matrix which will transform the near and far clipping planes to `[0, 1]`, respectively, along with its inverse.
	/// @param vertical_fov Vertical field of view angle, in radians.
	/// @param aspect_ratio Aspect ratio which determines the horizontal field of view.
	/// @param near Distance to the near clipping plane.
	/// @param far Distance to the far clipping plane.
	/// @return Tuple containing the perspective projection matrix, followed by its inverse.
	/// @note Constructing the inverse perspective projection matrix from projection parameters is faster and more precise than inverting matrix.
	template <class T>
	[[nodiscard]] std::tuple<mat4<T>, mat4<T>> perspective_half_z_inv(T vertical_fov, T aspect_ratio, T near, T far)
	{
		const T half_fov = vertical_fov * T{0.5};
		const T f = cos(half_fov) / sin(half_fov);

		return
		{
			mat4<T>
			{{
				{f / aspect_ratio, T{0}, T{0}, T{0}},
				{T{0}, f, T{0}, T{0}},
				{T{0}, T{0}, far / (near - far), T{-1}},
				{T{0}, T{0}, -(far * near) / (far - near), T{0}}
			}},

			mat4<T>
			{{
				{aspect_ratio / f, T{0}, T{0}, T{0}},
				{T{0}, T{1} / f, T{0}, T{0}},
				{T{0}, T{0}, T{0}, T{1} / far - T{1} / near},
				{T{0}, T{0}, T{-1}, T{1} / near}
			}}
		};
	}

	/// Constructs a perspective projection matrix, with an infinite far plane, which will transform the near and far clipping planes to `[1, 0]`, respectively.
	/// @param vertical_fov Vertical field of view angle, in radians.
	/// @param aspect_ratio Aspect ratio which determines the horizontal field of view.
	/// @param near Distance to the near clipping plane.
	/// @return Perspective projection matrix.
	template <class T>
	[[nodiscard]] mat4<T> inf_perspective_half_z_reverse(T vertical_fov, T aspect_ratio, T near)
	{
		const T half_fov = vertical_fov * T{0.5};
		const T f = cos(half_fov) / sin(half_fov);

		return
		{{
			{f / aspect_ratio, T{0}, T{0}, T{0}},
			{T{0}, f, T{0}, T{0}},
			{T{0}, T{0}, T{0}, T{-1}},
			{T{0}, T{0}, near, T{0}}
		}};
	}

	/// Constructs a perspective projection matrix, with an infinite far plane, which will transform the near and far clipping planes to `[1, 0]`, respectively, along with its inverse.
	/// @param vertical_fov Vertical field of view angle, in radians.
	/// @param aspect_ratio Aspect ratio which determines the horizontal field of view.
	/// @param near Distance to the near clipping plane.
	/// @return Tuple containing the perspective projection matrix, followed by its inverse.
	/// @note Constructing the inverse perspective projection matrix from projection parameters is faster and more precise than inverting matrix.
	template <class T>
	[[nodiscard]] std::tuple<mat4<T>, mat4<T>> inf_perspective_half_z_reverse_inv(T vertical_fov, T aspect_ratio, T near)
	{
		const T half_fov = vertical_fov * T{0.5};
		const T f = cos(half_fov) / sin(half_fov);

		return
		{
			mat4<T>
			{{
				{f / aspect_ratio, T{0}, T{0}, T{0}},
				{T{0}, f, T{0}, T{0}},
				{T{0}, T{0}, T{0}, T{-1}},
				{T{0}, T{0}, near, T{0}}
			}},

			mat4<T>
			{{
				{aspect_ratio / f, T{0}, T{0}, T{0}},
				{T{0}, T{1} / f, T{0}, T{0}},
				{T{0}, T{0}, T{0}, T{1} / near},
				{T{0}, T{0}, T{-1}, T{0}}
			}}
		};
	}
}

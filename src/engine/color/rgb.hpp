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

#ifndef ANTKEEPER_COLOR_RGB_HPP
#define ANTKEEPER_COLOR_RGB_HPP

#include <engine/color/cat.hpp>
#include <engine/math/matrix.hpp>
#include <engine/math/vector.hpp>

namespace color {

/// RGB color spaces.
namespace rgb {

/**
 * Constructs a matrix which transforms an RGB color into a CIE XYZ color.
 *
 * @param r CIE xy chromaticity coordinates of the red primary.
 * @param g CIE xy chromaticity coordinates of the green primary.
 * @param b CIE xy chromaticity coordinates of the blue primary.
 * @param w CIE xy chromaticity coordinates of the white point.
 *
 * @return Matrix which transforms an RGB color into a CIE XYZ color.
 *
 * @see https://www.ryanjuckett.com/rgb-color-space-conversion/
 * @see https://mina86.com/2019/srgb-xyz-matrix/
 */
template <class T>
[[nodiscard]] constexpr math::matrix<T, 3, 3> to_xyz(const math::vector2<T>& r, const math::vector2<T>& g, const math::vector2<T>& b, const math::vector2<T>& w)
{
	const math::matrix<T, 3, 3> m = 
	{
		r[0], r[1], T{1} - (r[0] + r[1]),
		g[0], g[1], T{1} - (g[0] + g[1]),
		b[0], b[1], T{1} - (b[0] + b[1])
	};
	
	const math::vector3<T> scale = math::inverse(m) * math::vector3<T>{w[0] / w[1], T{1}, (T{1} - (w[0] + w[1])) / w[1]};
	
	return math::matrix<T, 3, 3>
	{
		m[0][0] * scale[0], m[0][1] * scale[0], m[0][2] * scale[0],
		m[1][0] * scale[1], m[1][1] * scale[1], m[1][2] * scale[1],
		m[2][0] * scale[2], m[2][1] * scale[2], m[2][2] * scale[2],
	};
}

/**
 * RGB color space.
 */
template <class T>
struct color_space
{
	/// Transfer function function pointer type.
	typedef math::vector3<T> (*transfer_function_type)(const math::vector3<T>&);
	
	/// CIE xy chromaticity coordinates of the red primary.
	const math::vector2<T> r;
	
	/// CIE xy chromaticity coordinates of the green primary.
	const math::vector2<T> g;
	
	/// CIE xy chromaticity coordinates of the blue primary.
	const math::vector2<T> b;
	
	/// CIE xy chromaticity coordinates of the white point.
	const math::vector2<T> w;
	
	/// Function pointer to the electro-optical transfer function.
	const transfer_function_type eotf;
	
	/// Function pointer to the opto-electrical transfer function.
	const transfer_function_type oetf;
	
	/// Matrix which transforms an RGB color to a CIE XYZ color.
	const math::matrix3x3<T> to_xyz;
	
	/// Matrix which transforms a CIE XYZ color to an RGB color.
	const math::matrix3x3<T> from_xyz;
	
	/// Vector which gives the luminance of an RGB color via dot product.
	const math::vector3<T> to_y;
	
	/**
	 * Constructs an RGB color space.
	 *
	 * @param r CIE xy chromaticity coordinates of the red primary.
	 * @param g CIE xy chromaticity coordinates of the green primary.
	 * @param b CIE xy chromaticity coordinates of the blue primary.
	 * @param w CIE xy chromaticity coordinates of the white point.
	 */
	constexpr color_space(const math::vector2<T>& r, const math::vector2<T>& g, const math::vector2<T>& b, const math::vector2<T>& w, transfer_function_type eotf, transfer_function_type oetf):
		r(r),
		g(g),
		b(b),
		w(w),
		eotf(eotf),
		oetf(oetf),
		to_xyz(color::rgb::to_xyz<T>(r, g, b, w)),
		from_xyz(math::inverse(to_xyz)),
		to_y{to_xyz[0][1], to_xyz[1][1], to_xyz[2][1]}
	{}
	
	/**
	 * Measures the luminance of a linear RGB color.
	 *
	 * @param x Linear RGB color.
	 * @return return Luminance of @p x.
	 */
	[[nodiscard]] inline constexpr T luminance(const math::vector3<T>& x) const noexcept
	{
		return math::dot(x, to_y);
	}
};

/**
 * Constructs a matrix which transforms a color from one RGB color space to another RGB color space.
 *
 * @param s0 Source color space.
 * @param s1 Destination color space.
 * @param cone_response Chromatic adaptation transform cone response matrix.
 * 
 * @return Color space transformation matrix.
 */
template <class T>
[[nodiscard]] constexpr math::matrix3x3<T> to_rgb(const color_space<T>& s0, const color_space<T>& s1, const math::matrix3x3<T>& cone_response = color::cat::bradford<T>)
{
	return s1.from_xyz * color::cat::matrix(s0.w, s1.w, cone_response) * s0.to_xyz;
}

} // namespace rgb
} // namespace color

#endif // ANTKEEPER_COLOR_RGB_HPP

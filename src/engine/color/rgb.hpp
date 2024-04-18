// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_COLOR_RGB_HPP
#define ANTKEEPER_COLOR_RGB_HPP

#include <engine/color/cat.hpp>
#include <engine/color/xyy.hpp>
#include <engine/math/matrix.hpp>
#include <engine/math/vector.hpp>

namespace color {

/// @name RGB color spaces
/// @{

/**
 * Constructs a normalized primary matrix, which transforms a linear RGB color into a linear CIE XYZ color.
 *
 * @param r CIE xy chromaticity coordinates of the red primary.
 * @param g CIE xy chromaticity coordinates of the green primary.
 * @param b CIE xy chromaticity coordinates of the blue primary.
 * @param w CIE xy chromaticity coordinates of the white point.
 *
 * @return Matrix which transforms a linear RGB color into a linear CIE XYZ color.
 *
 * @see SMPTE RP-177:1993
 */
template <class T>
[[nodiscard]] constexpr math::mat3<T> normalized_primary_matrix(const math::vec2<T>& r, const math::vec2<T>& g, const math::vec2<T>& b, const math::vec2<T>& w)
{
	const math::mat3<T> primaries = 
	{
		r.x(), r.y(), T{1} - r.x() - r.y(),
		g.x(), g.y(), T{1} - g.x() - g.y(),
		b.x(), b.y(), T{1} - b.x() - b.y()
	};
	
	const math::vec3<T> scale = math::inverse(primaries) * xyy_to_xyz<T>({w.x(), w.y(), T{1}});
	
	return
	{
		primaries[0][0] * scale[0], primaries[0][1] * scale[0], primaries[0][2] * scale[0],
		primaries[1][0] * scale[1], primaries[1][1] * scale[1], primaries[1][2] * scale[1],
		primaries[2][0] * scale[2], primaries[2][1] * scale[2], primaries[2][2] * scale[2],
	};
}

/**
 * RGB color space.
 *
 * @see https://en.wikipedia.org/wiki/RGB_color_spaces
 */
template <class T>
struct rgb_color_space
{
	/** Transfer function function pointer type. */
	using transfer_function_type = math::vec3<T> (*)(const math::vec3<T>&);
	
	/** CIE xy chromaticity coordinates of the red, green, and blue primaries. */
	const math::mat3x2<T> primaries;
	
	/** CIE xy chromaticity coordinates of the white point. */
	const math::vec2<T> white_point;

	/** Encoding Color Component Transfer Function (CCTF). Encodes a linear tristimulus to a non-linear signal. */
	const transfer_function_type encoding_cctf;
	
	/** Decoding Color Component Transfer Function (CCTF). Decodes a linear tristimulus from a non-linear signal. */
	const transfer_function_type decoding_cctf;
	
	/** Normalized primary matrix, which transforms a linear RGB color into a linear CIE XYZ color. */
	const math::mat3<T> rgb_to_xyz_matrix;
	
	/** Inverse normalized primary matrix, which transforms a linear CIE XYZ color into a linear RGB color. */
	const math::mat3<T> xyz_to_rgb_matrix;
	
	/** Luminance coefficients. */
	const math::vec3<T> luma_coefficients;
	
	/**
	 * Constructs an RGB color space.
	 *
	 * @param r CIE xy chromaticity coordinates of the red primary.
	 * @param g CIE xy chromaticity coordinates of the green primary.
	 * @param b CIE xy chromaticity coordinates of the blue primary.
	 * @param w CIE xy chromaticity coordinates of the white point.
	 * @param encoding_cctf Encoding Color Component Transfer Function (CCTF).
	 * @param decoding_cctf Decoding Color Component Transfer Function (CCTF).
	 */
	constexpr rgb_color_space(const math::vec2<T>& r, const math::vec2<T>& g, const math::vec2<T>& b, const math::vec2<T>& w, transfer_function_type encoding_cctf, transfer_function_type decoding_cctf):
		primaries{r, g, b},
		white_point{w},
		encoding_cctf{encoding_cctf},
		decoding_cctf{decoding_cctf},
		rgb_to_xyz_matrix{normalized_primary_matrix<T>(r, g, b, w)},
		xyz_to_rgb_matrix{math::inverse(rgb_to_xyz_matrix)},
		luma_coefficients{rgb_to_xyz_matrix[0][1], rgb_to_xyz_matrix[1][1], rgb_to_xyz_matrix[2][1]}
	{}

	/**
	 * Encodes a linear tristimulus to a non-linear signal.
	 *
	 * @param x Linear tristimulus.
	 *
	 * @return Non-linear signal.
	 */
	[[nodiscard]] inline math::vec3<T> cctf_encode(const math::vec3<T>& x) const
	{
		return encoding_cctf ? encoding_cctf(x) : x;
	}

	/**
	 * Decodes a linear tristimulus from a non-linear signal.
	 *
	 * @param x Linear tristimulus.
	 *
	 * @return Non-linear signal.
	 */
	[[nodiscard]] inline math::vec3<T> cctf_decode(const math::vec3<T>& x) const
	{
		return decoding_cctf ? decoding_cctf(x) : x;
	}
	
	/**
	 * Transforms a linear RGB color into a linear CIE XYZ color.
	 *
	 * @param x Linear RGB color.
	 *
	 * @return Linear CIE XYZ color.
	 */
	[[nodiscard]] inline constexpr math::vec3<T> rgb_to_xyz(const math::vec3<T>& x) const noexcept
	{
		return rgb_to_xyz_matrix * x;
	}
	
	/**
	 * Transforms a linear CIE XYZ color into a linear RGB color.
	 *
	 * @param x Linear CIE XYZ color.
	 *
	 * @return Linear RGB color.
	 */
	[[nodiscard]] inline constexpr math::vec3<T> xyz_to_rgb(const math::vec3<T>& x) const noexcept
	{
		return xyz_to_rgb_matrix * x;
	}
	
	/**
	 * Measures the luminance of a linear RGB color.
	 *
	 * @param x Linear RGB color.
	 *
	 * @return return Luminance of @p x.
	 */
	[[nodiscard]] inline constexpr T luminance(const math::vec3<T>& x) const noexcept
	{
		return math::dot(x, luma_coefficients);
	}
};

/**
 * Constructs a matrix which transforms a linear tristimulus from one RGB color space to another RGB color space.
 *
 * @param src Source color space.
 * @param dst Destination color space.
 * @param cone_response Chromatic adaptation transform cone response matrix.
 * 
 * @return Color space transformation matrix.
 */
template <class T>
[[nodiscard]] constexpr math::mat3<T> rgb_to_rgb_matrix(const rgb_color_space<T>& src, const rgb_color_space<T>& dst, const math::mat3<T>& cone_response = bradford_cone_response<T>)
{
	if (src.white_point == dst.white_point)
	{
		return dst.xyz_to_rgb_matrix * src.rgb_to_xyz_matrix;
	}
	else
	{
		return dst.xyz_to_rgb_matrix * (cat_matrix(src.white_point, dst.white_point, cone_response) * src.rgb_to_xyz_matrix);
	}
}

/**
 * Transforms a color from one RGB color space to another RGB color space.
 *
 * @param rgb Input RGB values.
 * @param src_color_space Source color space.
 * @param dst_color_space Destination color space.
 * @param cctf_decode Decode input with the decoding CCTF of the source color space.
 * @param cctf_encode Encode output with the encoding CCTF of the destination color space.
 * @param cone_response Chromatic adaption transform cone response matrix.
 *
 * @return Output RGB values.
 */
template <class T>
[[nodiscard]] math::vec3<T> rgb_to_rgb(math::vec3<T> rgb, const rgb_color_space<T>& src_color_space, const rgb_color_space<T>& dst_color_space, bool cctf_decode = false, bool cctf_encode = false, const math::mat3<T>& cone_response = bradford_cone_response<T>)
{
	if (cctf_decode)
	{
		rgb = src_color_space.cctf_decode(rgb);
	}

	rgb = rgb_to_rgb_matrix(src_color_space, dst_color_space, cone_response) * rgb;

	if (cctf_encode)
	{
		rgb = dst_color_space.cctf_encode(rgb);
	}

	return rgb;
}

/// @}

} // namespace color

#endif // ANTKEEPER_COLOR_RGB_HPP

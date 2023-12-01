// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GL_BLEND_FACTOR_HPP
#define ANTKEEPER_GL_BLEND_FACTOR_HPP

#include <cstdint>

namespace gl {

/// Source and destination color and alpha blending factors.
enum class blend_factor: std::uint8_t
{
	zero,
	one,
	src_color,
	one_minus_src_color,
	dst_color,
	one_minus_dst_color,
	src_alpha,
	one_minus_src_alpha,
	dst_alpha,
	one_minus_dst_alpha,
	constant_color,
	one_minus_constant_color,
	constant_alpha,
	one_minus_constant_alpha,
	src_alpha_saturate,
	src1_color,
	one_minus_src1_color,
	src1_alpha,
	one_minus_src1_alpha
};

} // namespace gl

#endif // ANTKEEPER_GL_BLEND_FACTOR_HPP

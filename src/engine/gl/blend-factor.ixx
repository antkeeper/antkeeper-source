// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.gl.blend_factor;
import engine.utility.sized_types;

export namespace engine::gl
{
	/// Source and destination color and alpha blending factors.
	enum class blend_factor: u8
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
}

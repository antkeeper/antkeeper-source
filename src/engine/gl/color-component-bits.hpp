// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GL_COLOR_COMPONENT_BITS_HPP
#define ANTKEEPER_GL_COLOR_COMPONENT_BITS_HPP

#include <cstdint>

namespace gl {

/// Bits controlling which components are written to the framebuffer.
enum: std::uint8_t
{
	/// Indicates that the R value is written to the color attachment for the appropriate sample.
	color_component_r_bit = 0b0001,
	
	/// Indicates that the G value is written to the color attachment for the appropriate sample.
	color_component_g_bit = 0b0010,
	
	/// Indicates that the B value is written to the color attachment for the appropriate sample.
	color_component_b_bit = 0b0100,
	
	/// Indicates that the A value is written to the color attachment for the appropriate sample.
	color_component_a_bit = 0b1000,
};

} // namespace gl

#endif // ANTKEEPER_GL_COLOR_COMPONENT_BITS_HPP

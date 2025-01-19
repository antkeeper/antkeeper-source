// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GL_CLEAR_BITS_HPP
#define ANTKEEPER_GL_CLEAR_BITS_HPP

#include <cstdint>

namespace gl {

/// Clear mask bits.
enum: std::uint8_t
{
	/// Indicates the color buffer should be cleared.
	color_clear_bit = 0b001,
	
	/// Indicates the depth buffer should be cleared.
	depth_clear_bit = 0b010,
	
	/// Indicates the stencil buffer should be cleared.
	stencil_clear_bit = 0b100,
};

} // namespace gl

#endif // ANTKEEPER_GL_CLEAR_BITS_HPP

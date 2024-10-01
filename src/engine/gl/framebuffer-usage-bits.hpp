// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GL_FRAMEBUFFER_USAGE_BITS_HPP
#define ANTKEEPER_GL_FRAMEBUFFER_USAGE_BITS_HPP

#include <cstdint>

namespace gl {

/// Framebuffer attachment usage bits.
enum: std::uint8_t
{
	/// Framebuffer color attachment.
	color_attachment_bit = 0b001,
	
	/// Framebuffer depth attachment.
	depth_attachment_bit = 0b010,
	
	/// Framebuffer stencil attachment.
	stencil_attachment_bit = 0b100,
	
	/// Framebuffer depth/stencil attachment.
	depth_stencil_attachment_bits = 0b110,
};

} // namespace gl

#endif // ANTKEEPER_GL_FRAMEBUFFER_USAGE_BITS_HPP

// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GL_LOGIC_OP_HPP
#define ANTKEEPER_GL_LOGIC_OP_HPP

#include <cstdint>

namespace gl {

/// Framebuffer logical operations.
enum class logic_op: std::uint8_t
{
	bitwise_clear,
	bitwise_and,
	bitwise_and_reverse,
	bitwise_copy,
	bitwise_and_inverted,
	bitwise_no_op,
	bitwise_xor,
	bitwise_or,
	bitwise_nor,
	bitwise_equivalent,
	bitwise_invert,
	bitwise_or_reverse,
	bitwise_copy_inverted,
	bitwise_or_inverted,
	bitwise_nand,
	bitwise_set,
};

} // namespace gl

#endif // ANTKEEPER_GL_LOGIC_OP_HPP

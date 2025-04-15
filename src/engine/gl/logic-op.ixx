// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.gl.logic_op;
import engine.utility.sized_types;

export namespace engine::gl
{
	/// Framebuffer logical operations.
	enum class logic_op: u8
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
}

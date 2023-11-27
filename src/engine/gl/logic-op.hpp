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

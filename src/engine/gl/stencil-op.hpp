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

#ifndef ANTKEEPER_GL_STENCIL_OP_HPP
#define ANTKEEPER_GL_STENCIL_OP_HPP

#include <cstdint>

namespace gl {

/// Stencil comparison functions.
enum class stencil_op: std::uint8_t
{
	/// Keeps the current value.
    keep,
	
	/// Sets the value to `0`.
    zero,
	
	/// Sets the value to `reference`.
    replace,
	
	/// Increments the current value and clamps to the maximum representable unsigned value.
    increment_and_clamp,
	
	/// Decrements the current value and clamps to `0`.
    decrement_and_clamp,
	
	/// Bitwise-inverts the current value.
    invert,
	
	/// Increments the current value and wraps to `0` when the maximum value would have been exceeded.
    increment_and_wrap,
	
	/// Decrements the current value and wraps to the maximum possible value when the value would go below `0`.
    decrement_and_wrap
};

} // namespace gl

#endif // ANTKEEPER_GL_STENCIL_OP_HPP

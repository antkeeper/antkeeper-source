// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

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

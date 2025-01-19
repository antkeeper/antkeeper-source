// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GL_COMPARE_OP_HPP
#define ANTKEEPER_GL_COMPARE_OP_HPP

#include <cstdint>

namespace gl {

/// Comparison operators.
enum class compare_op: std::uint8_t
{
	/// Comparison always evaluates `false`.
    never,
	
	/// Comparison evaluates `reference` < `test`.
    less,
	
	/// Comparison evaluates `reference` == `test`.
    equal,
	
	/// Comparison evaluates `reference` <= `test`.
    less_or_equal,
	
	/// Comparison evaluates `reference` > `test`.
    greater,
	
	/// Comparison evaluates `reference` != `test`.
    not_equal,
	
	/// Comparison evaluates `reference` >= `test`.
    greater_or_equal,
	
	/// Comparison always evaluates `true`.
    always
};

} // namespace gl

#endif // ANTKEEPER_GL_COMPARE_OP_HPP

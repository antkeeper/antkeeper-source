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

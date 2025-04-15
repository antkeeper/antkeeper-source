// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.gl.compare_op;
import engine.utility.sized_types;

export namespace engine::gl
{
	/// Comparison operators.
	enum class compare_op: u8
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
}

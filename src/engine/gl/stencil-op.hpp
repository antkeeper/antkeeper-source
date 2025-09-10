// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <engine/utility/sized-types.hpp>

namespace engine::gl
{
	/// Stencil comparison functions.
	enum class stencil_op: u8
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
}

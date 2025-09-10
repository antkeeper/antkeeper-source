// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <engine/utility/sized-types.hpp>

namespace engine::gl
{
	/// Clear mask bits.
	enum: u8
	{
		/// Indicates the color buffer should be cleared.
		color_clear_bit = 0b001,

		/// Indicates the depth buffer should be cleared.
		depth_clear_bit = 0b010,

		/// Indicates the stencil buffer should be cleared.
		stencil_clear_bit = 0b100,
	};
}

// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <engine/utility/sized-types.hpp>

namespace engine::gl
{
	/// Bits controlling which components are written to the framebuffer.
	enum: u8
	{
		/// Indicates that the R value is written to the color attachment for the appropriate sample.
		color_component_r_bit = 0b0001,

		/// Indicates that the G value is written to the color attachment for the appropriate sample.
		color_component_g_bit = 0b0010,

		/// Indicates that the B value is written to the color attachment for the appropriate sample.
		color_component_b_bit = 0b0100,

		/// Indicates that the A value is written to the color attachment for the appropriate sample.
		color_component_a_bit = 0b1000,
	};
}

// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <engine/utility/sized-types.hpp>

namespace engine::gl
{
	enum class pixel_format: u8
	{
		/// Depth
		d,

		/// Depth, stencil
		ds,

		/// Red
		r,

		/// Red, green
		rg,

		/// Red, green, blue
		rgb,

		/// Blue, green, red
		bgr,

		/// Red, green, blue, alpha
		rgba,

		/// Blue, green, red, alpha
		bgra
	};
}

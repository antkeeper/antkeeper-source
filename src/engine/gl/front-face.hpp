// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <engine/utility/sized-types.hpp>

namespace engine::gl
{
	/// Polygon front-facing orientation.
	enum class front_face: u8
	{
		/// Triangle with positive area is considered front-facing.
		counter_clockwise,

		/// Triangle with negative area is considered front-facing.
		clockwise
	};
}

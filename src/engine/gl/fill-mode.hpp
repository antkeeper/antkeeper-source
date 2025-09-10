// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <engine/utility/sized-types.hpp>

namespace engine::gl
{
	/// Polygon rasterization mode.
	enum class fill_mode: u8
	{
		/// Polygons are filled.
		fill,

		/// Polygons edges are drawn as line segments.
		line,

		/// Polygons vertices are drawn as points.
		point
	};
}

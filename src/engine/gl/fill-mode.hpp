// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GL_FILL_MODE_HPP
#define ANTKEEPER_GL_FILL_MODE_HPP

#include <cstdint>

namespace gl {

/// Polygon rasterization mode.
enum class fill_mode: std::uint8_t
{
	/// Polygons are filled.
	fill,
	
	/// Polygons edges are drawn as line segments.
	line,
	
	/// Polygons vertices are drawn as points.
	point
};

} // namespace gl

#endif // ANTKEEPER_GL_FILL_MODE_HPP

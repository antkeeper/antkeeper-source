// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_TYPE_GLYPH_METRICS_HPP
#define ANTKEEPER_TYPE_GLYPH_METRICS_HPP

#include <engine/math/vector.hpp>

namespace type {

/**
 * Metrics describing properties of a glyph.
 */
struct glyph_metrics
{
	/// Horizontal extent of the glyph.
	float width;
	
	/// Vertical extent of the glyph.
	float height;
	
	/// Offset from the pen position to the glyph's top-left edge, in horizontal layouts.
	math::fvec2 horizontal_bearing;
	
	/// Offset from the pen position to the glph's top-left edge, in vertical layouts.
	math::fvec2 vertical_bearing;
	
	/// Distance to move the pen position after the glyph has been rendered, in horizontal layouts.
	float horizontal_advance;
	
	/// Distance to move the pen position after the glyph has been rendered, in vertical layouts.
	float vertical_advance;
};

} // namespace type

#endif // ANTKEEPER_TYPE_GLYPH_METRICS_HPP

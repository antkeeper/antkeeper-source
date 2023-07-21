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

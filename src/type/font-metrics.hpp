/*
 * Copyright (C) 2021  Christopher J. Howard
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

#ifndef ANTKEEPER_TYPE_FONT_METRICS_HPP
#define ANTKEEPER_TYPE_FONT_METRICS_HPP

namespace type {

/**
 * Metrics describing properties of a font.
 */
struct font_metrics
{
	/// Vertical size of the font, in pixels.
	float size;
	
	/// Positive distance from the baseline to the highest or upper grid coordinate.
	float ascent;
	
	/// Negative distance from the baseline to the lowest grid coordinate.
	float descent;
	
	/// Distance that must be placed between two lines of text.
	float linegap;
	
	/// Baseline-to-baseline distance, computed as `ascent - descent + linegap`.
	float linespace;
	
	/// Vertical position of an underline.
	float underline_position;
	
	/// Thickness of an underline.
	float underline_thickness;
	
	/// Maximum advance with for all glyphs, for horizontal layouts.
	float max_horizontal_advance;
	
	/// Maximum advance height for all glyphs, for vertical layouts.
	float max_vertical_advance;
};

} // namespace type

#endif // ANTKEEPER_TYPE_FONT_METRICS_HPP

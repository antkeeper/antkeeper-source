// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_TYPE_FONT_METRICS_HPP
#define ANTKEEPER_TYPE_FONT_METRICS_HPP

namespace type {

/// Metrics describing properties of a font.
struct font_metrics
{
	/// Vertical size of the font.
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

	/// EM square size.
	float em_size;
};

} // namespace type

#endif // ANTKEEPER_TYPE_FONT_METRICS_HPP

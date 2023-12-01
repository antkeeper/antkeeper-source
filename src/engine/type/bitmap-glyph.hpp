// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_TYPE_BITMAP_GLYPH_HPP
#define ANTKEEPER_TYPE_BITMAP_GLYPH_HPP

#include <engine/type/glyph-metrics.hpp>
#include <engine/math/vector.hpp>
#include <vector>
#include <cstddef>

namespace type {

/**
 * Single glyph in a bitmap font.
 *
 * @see type::bitmap_font
 */
struct bitmap_glyph
{
	/// Metrics describing the glyph.
	glyph_metrics metrics;
	
	/// Bitmap representing the glyph.
	std::vector<std::byte> bitmap;
	
	/// Width of the glyph bitmap, in pixels.
	std::uint32_t bitmap_width{};
	
	/// Height of the glyph bitmap, in pixels.
	std::uint32_t bitmap_height{};
	
	/// Position of the packed glyph bitmap within the font bitmap.
	math::uvec2 position;
};

} // namespace type

#endif // ANTKEEPER_TYPE_BITMAP_GLYPH_HPP

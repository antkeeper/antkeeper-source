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

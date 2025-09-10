// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <engine/math/vector.hpp>
#include <cstddef>
#include <memory>

namespace engine::type
{
	/// Representation of a single character in a font.
	struct glyph
	{
		/// Glyph dimensions, in fractional pixels.
		math::fvec2 dimensions{};
	
		/// Left and top side bearings of the glyph, for horizontal layouts, in fractional pixels.
		math::fvec2 horizontal_bearings{};
	
		/// Advance width of the glyph, for horizontal layouts, in fractional pixels.
		float horizontal_advance{};
	
		/// Left and top side bearings of the glyph, for vertical layouts, in fractional pixels.
		math::fvec2 vertical_bearings{};
	
		/// Advance height of the glyph, for vertical layouts, in fractional pixels.
		float vertical_advance{};
	
		/// Glyph bitmap position, within the font texture, in integer pixels.
		math::uvec2 bitmap_position{};
	
		/// Glyph bitmap dimensions, integer in pixels.
		math::uvec2 bitmap_dimensions{};
	
		/// Left and top side bearings of the glyph bitmap, in integer pixels.
		math::ivec2 bitmap_bearings{};
	
		/// Glyph bitmap data.
		std::unique_ptr<std::byte[]> bitmap_data;
	};
}

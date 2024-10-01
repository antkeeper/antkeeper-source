// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_TYPE_GLYPH_HPP
#define ANTKEEPER_TYPE_GLYPH_HPP

#include <array>
#include <cstddef>
#include <cstdint>
#include <memory>

namespace type {

/// Representation of a single character in a font.
struct glyph
{
	/// Glyph dimensions, in fractional pixels.
	std::array<float, 2> dimensions{};
	
	/// Left and top side bearings of the glyph, for horizontal layouts, in fractional pixels.
	std::array<float, 2> horizontal_bearings{};
	
	/// Advance width of the glyph, for horizontal layouts, in fractional pixels.
	float horizontal_advance{};
	
	/// Left and top side bearings of the glyph, for vertical layouts, in fractional pixels.
	std::array<float, 2> vertical_bearings{};
	
	/// Advance height of the glyph, for vertical layouts, in fractional pixels.
	float vertical_advance{};
	
	/// Glyph bitmap position, within the font texture, in integer pixels.
	std::array<std::uint32_t, 2> bitmap_position{};
	
	/// Glyph bitmap dimensions, integer in pixels.
	std::array<std::uint32_t, 2> bitmap_dimensions{};
	
	/// Left and top side bearings of the glyph bitmap, in integer pixels.
	std::array<std::int32_t, 2> bitmap_bearings{};
	
	/// Glyph bitmap data.
	std::unique_ptr<std::byte[]> bitmap_data;
};

} // namespace type

#endif // ANTKEEPER_TYPE_GLYPH_HPP

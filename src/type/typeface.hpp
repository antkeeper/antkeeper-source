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

#ifndef ANTKEEPER_TYPE_TYPEFACE_HPP
#define ANTKEEPER_TYPE_TYPEFACE_HPP

#include "type/font-metrics.hpp"
#include "type/glyph-metrics.hpp"
#include "resources/image.hpp"
#include "utility/fundamental-types.hpp"

namespace type {

/// Emumerates typeface styles.
enum class typeface_style
{
	/// Normal typeface style.
	normal,
	
	/// Italic typeface style.
	italic,
	
	/// Oblique typeface style.
	oblique
};

/**
 * Abstract base class for a typeface, which corresponds to a single digital font file.
 *
 * @see type::font
 */
class typeface
{
public:
	/**
	 * Creates a typeface, setting its style and weight.
	 *
	 * @param style Typeface style.
	 * @param weight Typeface weight.
	 */
	typeface(typeface_style style, int weight);
	
	/// Creates an empty typeface.
	typeface();
	
	/// Destroys a typeface.
	virtual ~typeface() = default;
	
	/**
	 * Sets the style of the typeface.
	 *
	 * @param style Typeface style.
	 */
	void set_style(typeface_style style);
	
	/**
	 * Sets the weight of the typeface.
	 *
	 * @param weight Typeface weight.
	 */
	void set_weight(int weight);
	
	/// Returns the style of the typeface.
	typeface_style get_style() const;
	
	/// Returns the weight of the typeface.
	int get_weight() const;
	
	/// Returns `true` if the typeface contains kerning information, `false` otherwise.
	virtual bool has_kerning() const = 0;
	
	/**
	 * Returns `true` if the typeface contains a glyph, `false` otherwise.
	 *
	 * @param code UTF-32 character code of a glyph.
	 * @return `true` if the typeface contains the glyph, `false` otherwise.
	 */
	virtual bool has_glyph(char32_t code) const = 0;
	
	/**
	 * Gets metrics for a font of the specified size.
	 *
	 * @param[in] height Height of the font, in pixels.
	 * @param[out] metrics Font metrics.
	 * @return `true` if font metrics were returned, `false` otherwise.
	 */
	virtual bool get_metrics(float height, font_metrics& metrics) const = 0;
	
	/**
	 * Gets metrics for a glyph in a font of the specified size.
	 *
	 * @param[in] height Height of the font, in pixels.
	 * @param[in] code UTF-32 character code of a glyph.
	 * @param[out] metrics Glyph metrics.
	 * @return `true` if glyph metrics were returned, `false` otherwise.
	 */
	virtual bool get_metrics(float height, char32_t code, glyph_metrics& metrics) const = 0;
	
	/**
	 * Gets a bitmap of a glyph in a font of the specified size.
	 *
	 * @param[in] height Height of the font, in pixels.
	 * @param[in] code UTF-32 character code of a glyph.
	 * @param[out] bitmap Glyph bitmap data.
	 * @return `true` if glyph bitmap data was returned, `false` otherwise.
	 */
	virtual bool get_bitmap(float height, char32_t code, image& bitmap) const = 0;
	
	/**
	 * Gets the kerning offset for a pair of glyphs.
	 *
	 * @param[in] height Height of the font, in pixels.
	 * @param[in] first UTF-32 character code of the first glyph.
	 * @param[in] second UTF-32 character code of the second glyph.
	 * @param[out] offset Kerning offset.
	 * @return `true` if a kerning offset was returned, `false` otherwise.
	 */
	virtual bool get_kerning(float height, char32_t first, char32_t second, float2& offset) const = 0;
	
private:
	typeface_style style;
	int weight;
};

inline typeface_style typeface::get_style() const
{
	return style;
}

inline int typeface::get_weight() const
{
	return weight;
}

} // namespace type

#endif // ANTKEEPER_TYPE_TYPEFACE_HPP

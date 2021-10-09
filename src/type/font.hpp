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

#ifndef ANTKEEPER_TYPE_FONT_HPP
#define ANTKEEPER_TYPE_FONT_HPP

#include "type/kerning-table.hpp"
#include "type/font-metrics.hpp"
#include "type/glyph-metrics.hpp"

namespace type {

/**
 * Abstract base class for fonts.
 *
 * @see type::font_metrics
 * @see type::glyph_metrics
 * @see type::bitmap_font
 */
class font
{
public:
	/**
	 * Creates a font and sets its metrics.
	 *
	 * @param metrics Metrics describing the font.
	 */
	font(const font_metrics& metrics);
	
	/// Creates an empty font.
	font();
	
	/// Destroys a font.
	virtual ~font();
	
	/**
	 * Returns `true` if the font contains a glyph with the given character code.
	 *
	 * @param code UTF-32 character code of a glyph.
	 * @return `true` if the font contains the glyph, `false` otherwise.
	 */
	virtual bool contains(char32_t code) const = 0;
	
	/**
	 * Sets the kerning offset for a pair of glyphs.
	 *
	 * @param first UTF-32 character code of the first glyph.
	 * @param second UTF-32 character code of the second glyph.
	 * @param offset Kerning offset.
	 */
	void kern(char32_t first, char32_t second, const float2& offset);
	
	/**
	 * Sets the font metrics
	 *
	 * @param metrics Font metrics.
	 */
	void set_font_metrics(const font_metrics& metrics);
	
	/**
	 * Returns metrics describing a glyph.
	 *
	 * @param code UTF-32 character code of a glyph.
	 * @return Metrics describing the glyph.
	 */
	virtual const glyph_metrics& get_glyph_metrics(char32_t code) const = 0;
	
	/**
	 * Returns the kerning offset for a pair of glyphs.
	 *
	 * @param first UTF-32 character code of the first glyph.
	 * @param second UTF-32 character code of the second glyph.
	 * @return Kerning offset.
	 */
	const float2& get_kerning(char32_t first, char32_t second) const;
	
	/// Returns the font's kerning table.
	const kerning_table& get_kerning_table() const;
	
	/// Returns metrics describing the font.
	const font_metrics& get_font_metrics() const;
	
protected:
	font_metrics metrics;
	kerning_table kerning_table;
};

inline const kerning_table& font::get_kerning_table() const
{
	return kerning_table;
}

inline const font_metrics& font::get_font_metrics() const
{
	return metrics;
}

} // namespace type

#endif // ANTKEEPER_TYPE_FONT_HPP

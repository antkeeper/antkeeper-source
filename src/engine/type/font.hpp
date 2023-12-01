// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_TYPE_FONT_HPP
#define ANTKEEPER_TYPE_FONT_HPP

#include <engine/type/kerning-table.hpp>
#include <engine/type/font-metrics.hpp>
#include <engine/type/glyph-metrics.hpp>

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
	void kern(char32_t first, char32_t second, const math::fvec2& offset);
	
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
	const math::fvec2& get_kerning(char32_t first, char32_t second) const;
	
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

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

#ifndef ANTKEEPER_TYPE_BITMAP_FONT_HPP
#define ANTKEEPER_TYPE_BITMAP_FONT_HPP

#include <engine/type/font.hpp>
#include <engine/type/bitmap-glyph.hpp>
#include <engine/gl/texture.hpp>
#include <unordered_map>

namespace type {

/**
 * Raster font in which glyphs are stored as arrays of pixels.
 *
 * @see type::font
 * @see type::font_metrics
 * @see type::bitmap_glyph
 * @see image
 */
class bitmap_font: public font
{
public:
	/**
	 * Creates a bitmap font and sets its metrics.
	 *
	 * @param metrics Metrics describing the font.
	 */
	explicit bitmap_font(const font_metrics& metrics);
	
	/// Creates an empty bitmap font.
	bitmap_font();
	
	/// Destroys a bitmap font.
	virtual ~bitmap_font() = default;
	
	/// @copydoc font::contains(char32_t) const
	virtual bool contains(char32_t code) const;
	
	/**
	 * Inserts a glyph into the font.
	 *
	 * @param code UTF-32 character code of the glyph to insert.
	 *
	 * @return Reference to the inserted glyph.
	 */
	bitmap_glyph& insert(char32_t code);
	
	/**
	 * Removes a glyph from the font.
	 *
	 * @param code UTF-32 character code of the glyph to remove.
	 */
	void remove(char32_t code);
	
	/**
	 * Removes all glyphs from the font.
	 */
	void clear();
	
	/**
	 * Packs all glyph bitmaps into the font bitmap.
	 *
	 * @param resize Automatically resize the font bitmap to contain all glyphs. Bitmap size will start at the closest power of two to the largest glyph, then its dimensions will increase to the next power of two until its large enough that all glyphs can be contained.
	 * @return `true` if all glyphs were successfully packed, `false` otherwise.
	 *
	 * @except std::runtime_error Glyph bitmap format doesn't match font bitmap format.
	 * @except std::runtime_error Not enough space in font bitmap to pack glyph.
	 */
	bool pack(bool resize = true);
	
	/**
	 * Unpacks all glyph bitmaps from the font bitmap.
	 *
	 * @param resize Automatically resizes the font bitmap to zero.
	 */
	void unpack(bool resize = true);
	
	/**
	 * @copydoc font::get_glyph_metrics(char32_t) const
	 *
	 * @except std::invalid_argument Cannot fetch metrics of unknown bitmap glyph
	 */
	virtual const glyph_metrics& get_glyph_metrics(char32_t code) const;
	
	/**
	 * Returns a pointer to the glyph corresponding to a UTF-32 character code, or `nullptr` if no such glyph was found.
	 *
	 * @param code UTF-32 character code of a glyph.
	 *
	 * @return Pointer to the corresponding glyph.
	 */
	/// @{
	[[nodiscard]] const bitmap_glyph* get_glyph(char32_t code) const;
	[[nodiscard]] bitmap_glyph* get_glyph(char32_t code);
	/// @}
	
	/// @copydoc bitmap_font::get_glyph(char32_t) const
	
	/// Returns the bitmap font's bitmap texture.
	[[nodiscard]] inline const std::shared_ptr<gl::texture_2d>& get_texture() const noexcept
	{
		return m_texture;
	}
	
private:
	std::unordered_map<char32_t, bitmap_glyph> m_glyphs;
	std::shared_ptr<gl::texture_2d> m_texture;
	std::shared_ptr<gl::sampler> m_sampler;
};

} // namespace type

#endif // ANTKEEPER_TYPE_BITMAP_FONT_HPP

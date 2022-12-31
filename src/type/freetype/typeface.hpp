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

#ifndef ANTKEEPER_TYPE_FREETYPE_TYPEFACE_HPP
#define ANTKEEPER_TYPE_FREETYPE_TYPEFACE_HPP

#include "type/typeface.hpp"
#include <ft2build.h>
#include FT_FREETYPE_H

namespace type {
namespace freetype {

/**
 * Typeface implementation using the FreeType library.
 *
 * @see type::typeface
 */
class typeface: public type::typeface
{
public:
	/**
	 * Creates a FreeType typeface.
	 *
	 * @param library Pointer to a FreeType library instance.
	 * @param face Pointer to the FreeType object instance.
	 * @param buffer Pointer to file buffer containing FreeType face data.
	 */
	typeface(FT_Library library, FT_Face face, unsigned char* buffer);
	
	/// Destroys a FreeType typeface.
	virtual ~typeface();
	
	/// @copydoc type::typeface::has_kerning() const
	virtual bool has_kerning() const;
	
	/// @copydoc type::typeface::has_glyph(char32_t) const
	virtual bool has_glyph(char32_t code) const;
	
	/// @copydoc type::typeface::get_metrics(float, font_metrics&) const
	virtual bool get_metrics(float height, font_metrics& metrics) const;
	
	/// @copydoc type::typeface::get_metrics(float, char32_t, glyph_metrics&) const
	virtual bool get_metrics(float height, char32_t code, glyph_metrics& metrics) const;
	
	/// @copydoc type::typeface::get_bitmap(float, char32_t, image&) const
	virtual bool get_bitmap(float height, char32_t code, image& bitmap) const;
	
	/// @copydoc type::typeface::get_kerning(float, char32_t, char32_t, float2&) const
	virtual bool get_kerning(float height, char32_t first, char32_t second, float2& offset) const;
	
private:
	void set_face_pixel_size(float height) const;
	
	FT_Library library;
	FT_Face face;
	unsigned char* buffer;
	mutable float height;
};

} // namespace freetype
} // namespace type

#endif // ANTKEEPER_TYPE_FREETYPE_TYPEFACE_HPP

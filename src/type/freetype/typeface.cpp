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

#include "type/freetype/typeface.hpp"
#include <stdexcept>
#include <string>

namespace type {
namespace freetype {

typeface::typeface(FT_Library library, FT_Face face, unsigned char* buffer):
	library(library),
	face(face),
	buffer(buffer),
	height(-1.0f)
{
	/// Build charset
	FT_UInt index;
	FT_ULong c = FT_Get_First_Char(face, &index);
	while (index)
	{
		this->charset.insert(static_cast<char32_t>(c));
		c = FT_Get_Next_Char(face, c, &index);
	}
}

typeface::~typeface()
{
	FT_Done_Face(face);
	delete[] buffer;
	FT_Done_FreeType(library);
}

bool typeface::has_kerning() const
{
	return FT_HAS_KERNING(face);
}

bool typeface::get_metrics(float height, font_metrics& metrics) const
{
	// Set font size
	set_face_pixel_size(height);
	
	// Get font metrics
	metrics.size = height;
	metrics.ascent = face->size->metrics.ascender / 64.0f;
	metrics.descent = face->size->metrics.descender / 64.0f;
	metrics.linespace = face->size->metrics.height / 64.0f;
	metrics.linegap = metrics.linespace - (metrics.ascent - metrics.descent);
	metrics.underline_position = FT_MulFix(face->underline_position, face->size->metrics.y_scale) / 64.0f;
	metrics.underline_thickness = FT_MulFix(face->underline_thickness, face->size->metrics.y_scale) / 64.0f;
	metrics.max_horizontal_advance = face->size->metrics.max_advance / 64.0f;
	metrics.max_vertical_advance = FT_MulFix(face->max_advance_height, face->size->metrics.y_scale) / 64.0f;
	
	return true;
}

bool typeface::get_metrics(float height, char32_t code, glyph_metrics& metrics) const
{
	// Set font size
	set_face_pixel_size(height);
	
	// Get index of glyph
	FT_UInt glyph_index = FT_Get_Char_Index(face, static_cast<FT_ULong>(code));
	
	// Load glyph and render bitmap
	FT_Error error = FT_Load_Glyph(face, glyph_index, FT_LOAD_DEFAULT);
	if (error)
	{
		throw std::runtime_error("FreeType failed to load glyph (error code " + std::to_string(error) + ")");
	}
	
	// Get glyph metrics
	metrics.width = face->glyph->metrics.width / 64.0f;
	metrics.height = face->glyph->metrics.height / 64.0f;
	metrics.horizontal_bearing.x() = face->glyph->metrics.horiBearingX / 64.0f;
	metrics.horizontal_bearing.y() = face->glyph->metrics.horiBearingY / 64.0f;
	metrics.vertical_bearing.x() = face->glyph->metrics.vertBearingX / 64.0f;
	metrics.vertical_bearing.y() = face->glyph->metrics.vertBearingY / 64.0f;
	metrics.horizontal_advance = face->glyph->metrics.horiAdvance / 64.0f;
	metrics.vertical_advance = face->glyph->metrics.vertAdvance / 64.0f;
	
	return true;
}

bool typeface::get_bitmap(float height, char32_t code, image& bitmap) const
{
	// Set font size
	set_face_pixel_size(height);
	
	// Get index of glyph
	FT_UInt glyph_index = FT_Get_Char_Index(face, static_cast<FT_ULong>(code));
	
	// Load glyph and render bitmap
	FT_Error error = FT_Load_Glyph(face, glyph_index, FT_LOAD_RENDER | FT_LOAD_TARGET_(FT_RENDER_MODE_NORMAL));
	if (error)
	{
		throw std::runtime_error("FreeType failed to load glyph (error code " + std::to_string(error) + ")");
	}
	
	// Format and resize bitmap
	bitmap.resize(0, 0);
	bitmap.format(sizeof(unsigned char), 1);
	bitmap.resize(face->glyph->bitmap.width, face->glyph->bitmap.rows);
	
	// Copy glyph bitmap data in bitmap
	std::memcpy(bitmap.data(), face->glyph->bitmap.buffer, bitmap.get_size());
	
	return true;
}

bool typeface::get_kerning(float height, char32_t first, char32_t second, float2& offset) const
{
	// Check if typeface has kerning information
	if (!has_kerning())
		return false;
	
	// Set font size
	set_face_pixel_size(height);
	
	// Get indices of the two glyphs
	FT_UInt first_index = FT_Get_Char_Index(face, static_cast<FT_ULong>(first));
	FT_UInt second_index = FT_Get_Char_Index(face, static_cast<FT_ULong>(second));
	
	// Get kerning vector
	FT_Vector kerning;
	FT_Error error = FT_Get_Kerning(face, first_index, second_index, FT_KERNING_DEFAULT, &kerning);
	if (error)
	{
		throw std::runtime_error("FreeType failed to get kerning vector (error code " + std::to_string(error) + ")");
	}
	
	offset = float2{static_cast<float>(kerning.x), static_cast<float>(kerning.y)} / 64.0f;
	
	return true;
}

void typeface::set_face_pixel_size(float height) const
{
	if (this->height == height)
		return;
	
	FT_Error error = FT_Set_Pixel_Sizes(face, 0, static_cast<FT_UInt>(height));
	if (error)
	{
		throw std::runtime_error("FreeType failed to set face size (error code " + std::to_string(error) + ")");
	}
	
	this->height = height;
}

} // namespace freetype
} // namespace type

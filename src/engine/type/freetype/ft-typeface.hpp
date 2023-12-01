// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_TYPE_FT_TYPEFACE_HPP
#define ANTKEEPER_TYPE_FT_TYPEFACE_HPP

#include <engine/type/typeface.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <vector>
#include <memory>

namespace type {

/**
 * Typeface implementation using the FreeType library.
 *
 * @see type::typeface
 */
class ft_typeface: public typeface
{
public:
	/**
	 * Creates a FreeType typeface.
	 *
	 * @param library Pointer to a FreeType library instance.
	 * @param face Pointer to the FreeType object instance.
	 * @param file_buffer File buffer containing FreeType face data.
	 */
	ft_typeface(FT_Library library, FT_Face face, std::unique_ptr<std::vector<FT_Byte>> file_buffer);
	
	/// Destroys a FreeType typeface.
	virtual ~ft_typeface();
	
	virtual bool has_kerning() const;
	virtual bool get_metrics(float height, font_metrics& metrics) const;
	virtual bool get_metrics(float height, char32_t code, glyph_metrics& metrics) const;
	virtual bool get_bitmap(float height, char32_t code, std::vector<std::byte>& bitmap, std::uint32_t& bitmap_width, std::uint32_t& bitmap_height) const;
	virtual bool get_kerning(float height, char32_t first, char32_t second, math::fvec2& offset) const;
	
private:
	void set_face_pixel_size(float height) const;
	
	FT_Library library;
	FT_Face face;
	std::unique_ptr<std::vector<FT_Byte>> file_buffer;
	mutable float height;
};

} // namespace type

#endif // ANTKEEPER_TYPE_FT_TYPEFACE_HPP

// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_TYPE_FT_TYPEFACE_HPP
#define ANTKEEPER_TYPE_FT_TYPEFACE_HPP

#include <ft2build.h>
#include FT_FREETYPE_H
#include <engine/type/typeface.hpp>
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
	 * Constructs a FreeType typeface.
	 *
	 * @param ft_library Pointer to a FreeType library instance.
	 * @param ft_face Pointer to the FreeType object instance.
	 * @param file_buffer File buffer containing FreeType face data.
	 */
	ft_typeface(FT_Library ft_library, FT_Face ft_face, std::unique_ptr<std::byte[]> file_buffer);
	
	/** Destructs a FreeType typeface. */
	~ft_typeface() override;
	
	[[nodiscard]] font_metrics get_font_metrics(float size) const override;
	[[nodiscard]] bool has_glyph(char32_t code) const override;
	[[nodiscard]] glyph get_glyph(char32_t code, float size, bool sdf) const override;
	[[nodiscard]] std::array<float, 2> get_kerning(float size, char32_t first, char32_t second) const override;
	
private:
	void set_face_pixel_size(float height) const;
	
	FT_Library m_ft_library;
	FT_Face m_ft_face;
	std::unique_ptr<std::byte[]> m_file_buffer;
	mutable float m_face_size{-1.0f};
};

} // namespace type

#endif // ANTKEEPER_TYPE_FT_TYPEFACE_HPP

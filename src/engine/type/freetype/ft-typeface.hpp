// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <ft2build.h>
#include <freetype/freetype.h>
#include <engine/type/typeface.hpp>
#include <engine/math/vector.hpp>
#include <cstddef>
#include <memory>
#include <vector>

namespace engine::type
{
	/// Typeface implementation using the FreeType library.
	/// @see type::typeface
	class ft_typeface: public typeface
	{
	public:
		/// Constructs a FreeType typeface.
		/// @param ft_library Pointer to a FreeType library instance.
		/// @param ft_face Pointer to the FreeType object instance.
		/// @param file_buffer File buffer containing FreeType face data.
		ft_typeface(FT_Library ft_library, FT_Face ft_face, std::unique_ptr<std::byte[]> file_buffer);
	
		/// Destructs a FreeType typeface.
		~ft_typeface() override;
	
		[[nodiscard]] font_metrics get_font_metrics(float size) const override;
		[[nodiscard]] bool has_glyph(char32_t code) const override;
		[[nodiscard]] glyph get_glyph(char32_t code, float size, bool sdf) const override;
		[[nodiscard]] math::fvec2 get_kerning(float size, char32_t first, char32_t second) const override;
	
	private:
		void set_face_pixel_size(float height) const;
	
		FT_Library m_ft_library;
		FT_Face m_ft_face;
		std::unique_ptr<std::byte[]> m_file_buffer;
		mutable float m_face_size{-1.0f};
	};
}

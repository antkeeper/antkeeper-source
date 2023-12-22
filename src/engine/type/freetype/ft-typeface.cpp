// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <engine/type/freetype/ft-typeface.hpp>
#include <engine/resources/resource-loader.hpp>
#include <engine/resources/deserialize-error.hpp>
#include <engine/debug/log.hpp>
#include <format>
#include <stdexcept>
#include <string>

namespace type {

ft_typeface::ft_typeface(FT_Library ft_library, FT_Face ft_face, std::unique_ptr<std::byte[]> file_buffer):
	m_ft_library(ft_library),
	m_ft_face(ft_face),
	m_file_buffer{std::move(file_buffer)}
{
	m_family_name = m_ft_face->family_name;
	m_style_name = m_ft_face->style_name;
	m_has_horizontal = FT_HAS_HORIZONTAL(m_ft_face);
	m_has_vertical = FT_HAS_VERTICAL(m_ft_face);
	m_has_kerning = FT_HAS_KERNING(m_ft_face);
	m_has_fixed_sizes = FT_HAS_FIXED_SIZES(m_ft_face);
	m_has_color = FT_HAS_COLOR(m_ft_face);
	m_is_scalable = FT_IS_SCALABLE(m_ft_face);
	m_is_fixed_width = FT_IS_FIXED_WIDTH(m_ft_face);
	
	m_style_flags = 0;
	if (m_ft_face->style_flags & FT_STYLE_FLAG_ITALIC)
	{
		m_style_flags |= typeface_style_italic_bit;
	}
	if (m_ft_face->style_flags & FT_STYLE_FLAG_BOLD)
	{
		m_style_flags |= typeface_style_bold_bit;
	}
}

ft_typeface::~ft_typeface()
{
	FT_Done_Face(m_ft_face);
	FT_Done_FreeType(m_ft_library);
}

font_metrics ft_typeface::get_font_metrics(float size) const
{
	// Set font size
	set_face_pixel_size(size);
	
	// Get font metrics
	font_metrics metrics;
	metrics.size = size;
	metrics.ascent = m_ft_face->size->metrics.ascender / 64.0f;
	metrics.descent = m_ft_face->size->metrics.descender / 64.0f;
	metrics.linespace = m_ft_face->size->metrics.height / 64.0f;
	metrics.linegap = metrics.linespace - (metrics.ascent - metrics.descent);
	metrics.underline_position = FT_MulFix(m_ft_face->underline_position, m_ft_face->size->metrics.y_scale) / 64.0f;
	metrics.underline_thickness = FT_MulFix(m_ft_face->underline_thickness, m_ft_face->size->metrics.y_scale) / 64.0f;
	metrics.max_horizontal_advance = m_ft_face->size->metrics.max_advance / 64.0f;
	metrics.max_vertical_advance = FT_MulFix(m_ft_face->max_advance_height, m_ft_face->size->metrics.y_scale) / 64.0f;
	
	return metrics;
}

bool ft_typeface::has_glyph(char32_t code) const
{
	return FT_Get_Char_Index(m_ft_face, static_cast<FT_ULong>(code));
}

glyph ft_typeface::get_glyph(char32_t code, float size, bool sdf) const
{
	// Set font size
	set_face_pixel_size(size);
	
	// Get index of glyph from character code
	const FT_UInt glyph_index = FT_Get_Char_Index(m_ft_face, static_cast<FT_ULong>(code));
	
	// Determine glyph load flags
	FT_Int32 ft_load_flags = FT_LOAD_RENDER;
	if (sdf)
	{
		ft_load_flags |= FT_LOAD_TARGET_(FT_RENDER_MODE_SDF);
	}
	else
	{
		ft_load_flags |= FT_LOAD_TARGET_NORMAL;
	}
	
	// Load glyph and render bitmap
	if (const FT_Error error = FT_Load_Glyph(m_ft_face, glyph_index, ft_load_flags))
	{
		throw std::runtime_error(std::format("FreeType failed to load glyph (error code \"{}\")", error));
	}
	
	// Allocate glyph
	glyph g;
	
	// Calculate glyph metrics, in pixels
	g.dimensions[0] = m_ft_face->glyph->metrics.width / 64.0f;
	g.dimensions[1] = m_ft_face->glyph->metrics.height / 64.0f;
	g.horizontal_bearings[0] = m_ft_face->glyph->metrics.horiBearingX / 64.0f;
	g.horizontal_bearings[1] = m_ft_face->glyph->metrics.horiBearingY / 64.0f;
	g.horizontal_advance = m_ft_face->glyph->metrics.horiAdvance / 64.0f;
	g.vertical_bearings[0] = m_ft_face->glyph->metrics.vertBearingX / 64.0f;
	g.vertical_bearings[1] = m_ft_face->glyph->metrics.vertBearingY / 64.0f;
	g.vertical_advance = m_ft_face->glyph->metrics.vertAdvance / 64.0f;
	g.bitmap_dimensions[0] = static_cast<std::uint32_t>(m_ft_face->glyph->bitmap.width);
	g.bitmap_dimensions[1] = static_cast<std::uint32_t>(m_ft_face->glyph->bitmap.rows);
	g.bitmap_bearings[0] = static_cast<std::int32_t>(m_ft_face->glyph->bitmap_left);
	g.bitmap_bearings[1] = static_cast<std::int32_t>(m_ft_face->glyph->bitmap_top);
	
	// Allocate and copy glyph bitmap
	g.bitmap_data = std::make_unique<std::byte[]>(g.bitmap_dimensions[0] * g.bitmap_dimensions[1]);
	std::memcpy(g.bitmap_data.get(), m_ft_face->glyph->bitmap.buffer, g.bitmap_dimensions[0] * g.bitmap_dimensions[1]);
	
	return g;
}

std::array<float, 2> ft_typeface::get_kerning(float size, char32_t first, char32_t second) const
{
	if (!has_kerning())
	{
		return {};
	}
	
	// Set font size
	set_face_pixel_size(size);
	
	// Get indices of the two glyphs from their character codes
	const FT_UInt first_index = FT_Get_Char_Index(m_ft_face, static_cast<FT_ULong>(first));
	const FT_UInt second_index = FT_Get_Char_Index(m_ft_face, static_cast<FT_ULong>(second));
	
	// Get kerning vector
	FT_Vector kerning;
	if (const FT_Error error = FT_Get_Kerning(m_ft_face, first_index, second_index, FT_KERNING_DEFAULT, &kerning))
	{
		throw std::runtime_error(std::format("FreeType failed to get kerning vector (error code \"{}\")", error));
	}
	
	// Calculate pixel offset from kerning vector
	return
	{
		static_cast<float>(kerning.x) / 64.0f,
		static_cast<float>(kerning.y) / 64.0f
	};
}

void ft_typeface::set_face_pixel_size(float size) const
{
	if (m_face_size != size)
	{
		if (const FT_Error error = FT_Set_Pixel_Sizes(m_ft_face, 0, static_cast<FT_UInt>(std::round(size))))
		{
			throw std::runtime_error(std::format("FreeType failed to set face size (error code \"{}\")", error));
		}
		
		m_face_size = size;
	}
}

} // namespace type

template <>
std::unique_ptr<type::typeface> resource_loader<type::typeface>::load([[maybe_unused]] ::resource_manager& resource_manager, deserialize_context& ctx)
{
	// Init FreeType library object
	FT_Library ft_library;
	if (FT_Error error = FT_Init_FreeType(&ft_library))
	{
		throw std::runtime_error(std::format("Failed to init FreeType library (error code \"{}\")", error));
	}
	
	// Read file into file buffer
	auto file_buffer = std::make_unique<std::byte[]>(ctx.size());
	ctx.read8(file_buffer.get(), ctx.size());
	
	// Load FreeType face from file buffer
	FT_Face ft_face;
	if (const FT_Error error = FT_New_Memory_Face(ft_library, reinterpret_cast<const FT_Byte*>(file_buffer.get()), static_cast<FT_Long>(ctx.size()), 0, &ft_face))
	{
		FT_Done_FreeType(ft_library);
		throw deserialize_error(std::format("Failed to load FreeType face (error code \"{}\")", error));
	}
	
	return std::make_unique<type::ft_typeface>(ft_library, ft_face, std::move(file_buffer));
}

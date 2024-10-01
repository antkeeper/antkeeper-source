// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_TYPE_FONT_HPP
#define ANTKEEPER_TYPE_FONT_HPP

#include <engine/type/font-metrics.hpp>
#include <engine/type/typeface.hpp>
#include <engine/geom/rect-pack.hpp>
#include <engine/gl/texture.hpp>
#include <engine/event/publisher.hpp>
#include <array>
#include <memory>
#include <span>
#include <string_view>
#include <unordered_map>

namespace type {

class font;

/// Event generated when a font texture has been resized.
struct font_texture_resized_event
{
	/// Font that generated the event.
	type::font* font{};
};

/// Font.
class font
{
public:
	/// Constructs a font.
	/// @param face Typeface to which the font belongs.
	/// @param size Size of the font, in pixels.
	/// @param sdf `true` if the font should render signed distance field (SDF) glyphs, `false` otherwise.
	font(std::shared_ptr<typeface> face, float size, bool sdf = false);
	
	/// @name Glyphs
	/// @{
	
	/// Caches a single glyph.
	/// @param code UTF-32 character code of a glyph.
	/// @return Number of newly-cached glyphs.
	/// @warning Font texture view and texture image may be reconstructed.
	/// @warning Texture coordinates of pre-cached glyphs may be modified.
	/// @note Caching groups of glyphs with the cache_glyphs() functions is more efficient.
	std::size_t cache_glyph(char32_t code);
	
	/// Caches a range of glyphs.
	/// @param first UTF-32 character code of the first glyph in the range.
	/// @param last UTF-32 character code of the last glyph in the range.
	/// @return Number of newly-cached glyphs.
	/// @warning Font texture view and texture image may be reconstructed.
	/// @warning Texture coordinates of pre-cached glyphs may be modified.
	std::size_t cache_glyphs(char32_t first, char32_t last);
	
	/// Caches all glyphs required to render a string of text.
	/// @param text UTF-32 text.
	/// @return Number of newly-cached glyphs.
	/// @warning Font texture view and texture image may be reconstructed.
	/// @warning Texture coordinates of pre-cached glyphs may be modified.
	std::size_t cache_glyphs(std::u32string_view text);
	
	/// Caches all glyphs requried to render a string of text.
	/// @param text UTF-8 text.
	/// @return Number of newly-cached glyphs.
	/// @warning Font texture view and texture image may be reconstructed.
	/// @warning Texture coordinates of pre-cached glyphs may be modified.
	std::size_t cache_glyphs(std::string_view text);
	
	/// Returns a pointer to the cached glyph for the given character code.
	/// @param code UTF-32 character code of a glyph.
	/// @return Pointer to the corresponding cached glyph, or `nullptr` if the glyph has not been cached.
	/// @note If the font's typeface does not contain the given character, the glyph for character code 0 will be returned.
	[[nodiscard]] const glyph* get_cached_glyph(char32_t code) const;
	
	/// Gets the kerning offset for a pair of glyphs.
	/// @param first UTF-32 character code of the first glyph.
	/// @param second UTF-32 character code of the second glyph.
	/// @return Kerning offset, in pixels.
	[[nodiscard]] std::array<float, 2> get_kerning(char32_t first, char32_t second) const;
	
	/// @}
	
	/// Returns the typeface to which the font belongs.
	[[nodiscard]] inline constexpr auto& get_typeface() const noexcept
	{
		return m_typeface;
	}
	
	/// Returns metrics describing the font.
	[[nodiscard]] inline constexpr const auto& get_metrics() const noexcept
	{
		return m_metrics;
	}
	
	/// Returns the texture containing font glyphs.
	[[nodiscard]] inline constexpr const auto& get_texture() const noexcept
	{
		return m_texture;
	}
	
	/// Returns the channel through which font texture resized events are published.
	[[nodiscard]] inline auto& get_texture_resized_channel() noexcept
	{
		return m_texture_resized_publisher.channel();
	}
	
private:
	/// Writes glyph bitmaps to the font texture.
	/// @param glyphs Glyphs for which bitmaps should be written.
	void write_bitmaps(std::span<glyph*> glyphs);
	
	/// Resizes the font texture size and repacks glyph bitmaps.
	void repack_glyphs();
	
	std::shared_ptr<typeface> m_typeface;
	float m_size{};
	bool m_sdf{};
	font_metrics m_metrics;
	std::shared_ptr<gl::texture_2d> m_texture;
	std::unordered_map<char32_t, glyph> m_glyph_map;
	geom::rect_pack<std::uint32_t> m_glyph_pack;
	event::publisher<font_texture_resized_event> m_texture_resized_publisher;
};

} // namespace type

#endif // ANTKEEPER_TYPE_FONT_HPP

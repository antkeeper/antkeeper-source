// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.type.font;
export import engine.type.glyph;
export import engine.event.publisher;
export import engine.math.vector;
import engine.geom.rect_pack;
import engine.gl.texture;
import engine.utility.sized_types;
export import <memory>;
export import <string_view>;
import <span>;
import <unordered_map>;

export namespace engine::type
{
	class font;
	class typeface;

	/// Event generated when a font texture has been resized.
	struct font_texture_resized_event
	{
		/// Font that generated the event.
		type::font* font{};
	};

	/// Metrics describing properties of a font.
	struct font_metrics
	{
		/// Vertical size of the font.
		float size;

		/// Positive distance from the baseline to the highest or upper grid coordinate.
		float ascent;

		/// Negative distance from the baseline to the lowest grid coordinate.
		float descent;

		/// Distance that must be placed between two lines of text.
		float linegap;

		/// Baseline-to-baseline distance, computed as `ascent - descent + linegap`.
		float linespace;

		/// Vertical position of an underline.
		float underline_position;

		/// Thickness of an underline.
		float underline_thickness;

		/// Maximum advance with for all glyphs, for horizontal layouts.
		float max_horizontal_advance;

		/// Maximum advance height for all glyphs, for vertical layouts.
		float max_vertical_advance;

		/// EM square size.
		float em_size;
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
		usize cache_glyph(char32_t code);
	
		/// Caches a range of glyphs.
		/// @param first UTF-32 character code of the first glyph in the range.
		/// @param last UTF-32 character code of the last glyph in the range.
		/// @return Number of newly-cached glyphs.
		/// @warning Font texture view and texture image may be reconstructed.
		/// @warning Texture coordinates of pre-cached glyphs may be modified.
		usize cache_glyphs(char32_t first, char32_t last);
	
		/// Caches all glyphs required to render a string of text.
		/// @param text UTF-32 text.
		/// @return Number of newly-cached glyphs.
		/// @warning Font texture view and texture image may be reconstructed.
		/// @warning Texture coordinates of pre-cached glyphs may be modified.
		usize cache_glyphs(std::u32string_view text);
	
		/// Caches all glyphs required to render a string of text.
		/// @param text UTF-8 text.
		/// @return Number of newly-cached glyphs.
		/// @warning Font texture view and texture image may be reconstructed.
		/// @warning Texture coordinates of pre-cached glyphs may be modified.
		usize cache_glyphs(std::string_view text);
	
		/// Returns a pointer to the cached glyph for the given character code.
		/// @param code UTF-32 character code of a glyph.
		/// @return Pointer to the corresponding cached glyph, or `nullptr` if the glyph has not been cached.
		/// @note If the font's typeface does not contain the given character, the glyph for character code 0 will be returned.
		[[nodiscard]] const glyph* get_cached_glyph(char32_t code) const;
	
		/// Gets the kerning offset for a pair of glyphs.
		/// @param first UTF-32 character code of the first glyph.
		/// @param second UTF-32 character code of the second glyph.
		/// @return Kerning offset, in pixels.
		[[nodiscard]] math::fvec2 get_kerning(char32_t first, char32_t second) const;
	
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
		geom::rect_pack<u32> m_glyph_pack;
		event::publisher<font_texture_resized_event> m_texture_resized_publisher;
	};
}

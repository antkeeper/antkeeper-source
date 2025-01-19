// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_TYPE_TYPEFACE_HPP
#define ANTKEEPER_TYPE_TYPEFACE_HPP

#include <engine/type/font-metrics.hpp>
#include <engine/type/glyph.hpp>
#include <engine/type/typeface-style.hpp>
#include <array>
#include <string>

namespace type {

/// Abstract base class for a typeface, which corresponds to a single digital font file.
/// @see type::font
class typeface
{
public:
	/// Destructs a typeface.
	virtual ~typeface() = default;
	
	/// @name Font
	/// @{
	
	/// Gets the metrics for a font of the given size.
	/// @param size Font size, in pixels.
	/// @return Font metrics.
	[[nodiscard]] virtual font_metrics get_font_metrics(float size) const = 0;
	
	/// @}
	
	/// @name Glyphs
	/// @{
	
	/// Checks if the typeface contains the given glyph.
	/// @param code UTF-32 character code of a glyph.
	/// @return `true` if the typeface contains the glyph, `false` otherwise.
	[[nodiscard]] virtual bool has_glyph(char32_t code) const = 0;
	
	/// Loads a glyph.
	/// @param code UTF-32 character code of a glyph.
	/// @param size Font size, in pixels.
	/// @param sdf `true` to render a signed distance field (SDF) glyph bitmap, `false` otherwise.
	/// @return Loaded glyph.
	[[nodiscard]] virtual glyph get_glyph(char32_t code, float size, bool sdf = false) const = 0;
	
	/// @}
	
	/// @name Kerning
	/// @{
	
	/// Gets the kerning offset for a pair of glyphs.
	/// @param size Font size, in pixels.
	/// @param first UTF-32 character code of the first glyph.
	/// @param second UTF-32 character code of the second glyph.
	/// @return Kerning offset, in pixels.
	[[nodiscard]] virtual std::array<float, 2> get_kerning(float size, char32_t first, char32_t second) const = 0;
	
	/// @}
	
	/// @name Typeface information
	/// @{
	
	/// Returns the name of the typeface family, if any.
	[[nodiscard]] inline constexpr const auto& get_family_name() const noexcept
	{
		return m_family_name;
	}
	
	/// Returns the name of the typeface style, if any.
	[[nodiscard]] inline constexpr const auto& get_style_name() const noexcept
	{
		return m_style_name;
	}
	
	/// Returns `true` if the typeface has horizontal metrics, `false` otherwise.
	[[nodiscard]] inline constexpr auto has_horizontal() const noexcept
	{
		return m_has_horizontal;
	}
	
	/// Returns `true` if the typeface has vertical metrics, `false` otherwise.
	[[nodiscard]] inline constexpr auto has_vertical() const noexcept
	{
		return m_has_vertical;
	}
	
	/// Returns `true` if the typeface has kerning information, `false` otherwise.
	[[nodiscard]] inline constexpr auto has_kerning() const noexcept
	{
		return m_has_kerning;
	}
	
	/// Returns `true` if the typeface has fixed font sizes, `false` otherwise.
	[[nodiscard]] inline constexpr auto has_fixed_sizes() const noexcept
	{
		return m_has_fixed_sizes;
	}
	
	/// Returns `true` if the typeface has colored glyphs, `false` otherwise.
	[[nodiscard]] inline constexpr auto has_color() const noexcept
	{
		return m_has_color;
	}
	
	/// Returns `true` if the typeface has scalable font sizes, `false` otherwise.
	[[nodiscard]] inline constexpr auto is_scalable() const noexcept
	{
		return m_is_scalable;
	}
	
	/// Returns `true` if the typeface has fixed-width (monospace) glyphs, `false` otherwise.
	[[nodiscard]] inline constexpr auto is_fixed_width() const noexcept
	{
		return m_is_fixed_width;
	}
	
	/// Returns the style flags of the typeface.
	[[nodiscard]] inline constexpr auto get_style_flags() const noexcept
	{
		return m_style_flags;
	}
	
	/// Returns `true` if the typeface is italic, `false` otherwise.
	[[nodiscard]] inline constexpr auto is_italic() const noexcept
	{
		return m_style_flags & typeface_style_italic_bit;
	}
	
	/// Returns `true` if the typeface is bold, `false` otherwise.
	[[nodiscard]] inline constexpr auto is_bold() const noexcept
	{
		return m_style_flags & typeface_style_bold_bit;
	}
	
	/// @}
	
protected:
	std::string m_family_name;
	std::string m_style_name;
	bool m_has_horizontal{false};
	bool m_has_vertical{false};
	bool m_has_kerning{false};
	bool m_has_fixed_sizes{false};
	bool m_has_color{false};
	bool m_is_scalable{false};
	bool m_is_fixed_width{false};
	std::uint32_t m_style_flags{};
};

} // namespace type

#endif // ANTKEEPER_TYPE_TYPEFACE_HPP

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

#include "game/fonts.hpp"
#include "type/type.hpp"
#include "resources/resource-manager.hpp"
#include "gl/texture-wrapping.hpp"
#include "gl/texture-filter.hpp"
#include "render/material.hpp"
#include "render/material-flags.hpp"
#include "utility/hash/fnv1a.hpp"
#include "game/strings.hpp"
#include <codecvt>

using namespace hash::literals;

namespace game {

static void build_bitmap_font(const type::typeface& typeface, float size, const std::unordered_set<char32_t>& charset, type::bitmap_font& font, render::material& material, gl::shader_program* shader_program)
{
	// Get font metrics for given size
	if (type::font_metrics metrics; typeface.get_metrics(size, metrics))
		font.set_font_metrics(metrics);
	
	// Format font bitmap
	image& font_bitmap = font.get_bitmap();
	font_bitmap.format(sizeof(std::byte), 1);
	
	// For each UTF-32 character code in the character set
	for (char32_t code: charset)
	{
		// Skip missing glyphs
		if (!typeface.has_glyph(code))
			continue;
		
		// Add glyph to font
		type::bitmap_glyph& glyph = font[code];
		typeface.get_metrics(size, code, glyph.metrics);
		typeface.get_bitmap(size, code, glyph.bitmap);
	}
	
	// Pack glyph bitmaps into the font bitmap
	font.pack();
	
	// Create font texture from bitmap
	gl::texture_2d* font_texture = new gl::texture_2d(font_bitmap.get_width(), font_bitmap.get_height(), gl::pixel_type::uint_8, gl::pixel_format::r, gl::color_space::linear, font_bitmap.data());
	font_texture->set_wrapping(gl::texture_wrapping::extend, gl::texture_wrapping::extend);
	font_texture->set_filters(gl::texture_min_filter::linear, gl::texture_mag_filter::linear);
	
	// Create font material
	material.set_blend_mode(render::blend_mode::translucent);
	material.add_property<const gl::texture_2d*>("font_bitmap")->set_value(font_texture);
	material.set_shader_program(shader_program);
}

void load_fonts(game::context& ctx)
{
	// Load dyslexia-friendly typeface (if enabled)
	bool dyslexia_font_loaded = false;
	if (ctx.dyslexia_font)
	{
		const auto dyslexia_font_path = get_string(ctx, "font_dyslexia"_fnv1a32);
		ctx.typefaces["dyslexia"] = ctx.resource_manager->load<type::typeface>(dyslexia_font_path);
		dyslexia_font_loaded = true;
	}
	
	// Load typefaces
	if (dyslexia_font_loaded)
	{
		// Override standard typefaces with dyslexia-friendly typeface
		ctx.typefaces["serif"] = ctx.typefaces["dyslexia"];
		ctx.typefaces["sans_serif"] = ctx.typefaces["dyslexia"];
		ctx.typefaces["monospace"] = ctx.typefaces["dyslexia"];
	}
	else
	{
		// Load standard typefaces
		const auto serif_font_path = get_string(ctx, "font_serif"_fnv1a32);
		const auto sans_serif_font_path = get_string(ctx, "font_sans_serif"_fnv1a32);
		const auto monospace_font_path = get_string(ctx, "font_monospace"_fnv1a32);
		
		ctx.typefaces["serif"] = ctx.resource_manager->load<type::typeface>(serif_font_path);
		ctx.typefaces["sans_serif"] = ctx.resource_manager->load<type::typeface>(sans_serif_font_path);
		ctx.typefaces["monospace"] = ctx.resource_manager->load<type::typeface>(monospace_font_path);
	}
	
	// Build character set
	std::unordered_set<char32_t> charset;
	{
		// Add all character codes from the basic Latin unicode block
		for (char32_t code = type::unicode::block::basic_latin.first; code <= type::unicode::block::basic_latin.last; ++code)
			charset.insert(code);
		
		// Add all character codes from game strings
		const auto& string_map = ctx.string_maps[ctx.language_index];
		for (auto it = string_map.begin(); it != string_map.end(); ++it)
		{
			// Convert UTF-8 string to UTF-32
			std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> convert;
			std::u32string u32 = convert.from_bytes(it->second);
			
			/// Insert each character code from the UTF-32 string into the character set
			for (char32_t code: u32)
				charset.insert(code);
		}
	}
	
	// Load bitmap font shader
	gl::shader_program* bitmap_font_shader = ctx.resource_manager->load<gl::shader_program>("bitmap-font.glsl");
	
	// Point size to pixel size conversion factor
	const float dpi = ctx.window_manager->get_display(0).get_dpi();
	const float pt_to_px = (dpi / 72.0f) * ctx.font_scale;
	
	// Build debug font
	if (auto it = ctx.typefaces.find("monospace"); it != ctx.typefaces.end())
	{
		build_bitmap_font(*it->second, ctx.debug_font_size_pt * pt_to_px, charset, ctx.debug_font, ctx.debug_font_material, bitmap_font_shader);
	}
	
	// Build menu font
	if (auto it = ctx.typefaces.find("sans_serif"); it != ctx.typefaces.end())
	{
		build_bitmap_font(*it->second, ctx.menu_font_size_pt * pt_to_px, charset, ctx.menu_font, ctx.menu_font_material, bitmap_font_shader);
	}
	
	// Build title font
	if (auto it = ctx.typefaces.find("serif"); it != ctx.typefaces.end())
	{
		build_bitmap_font(*it->second, ctx.title_font_size_pt * pt_to_px, charset, ctx.title_font, ctx.title_font_material, bitmap_font_shader);
	}
}

} // namespace game

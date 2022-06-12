/*
 * Copyright (C) 2021  Christopher J. Howard
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
#include "application.hpp"
#include "type/type.hpp"
#include "resources/resource-manager.hpp"
#include "gl/texture-wrapping.hpp"
#include "gl/texture-filter.hpp"
#include "render/material.hpp"
#include "render/material-flags.hpp"
#include <codecvt>

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
	gl::texture_2d* font_texture = new gl::texture_2d(font_bitmap.get_width(), font_bitmap.get_height(), gl::pixel_type::uint_8, gl::pixel_format::r, gl::color_space::linear, font_bitmap.get_pixels());
	font_texture->set_wrapping(gl::texture_wrapping::extend, gl::texture_wrapping::extend);
	font_texture->set_filters(gl::texture_min_filter::linear, gl::texture_mag_filter::linear);
	
	// Create font material
	material.set_flags(MATERIAL_FLAG_TRANSLUCENT);
	material.add_property<const gl::texture_2d*>("font_bitmap")->set_value(font_texture);
	material.set_shader_program(shader_program);
}

void load_fonts(game::context* ctx)
{
	// Load dyslexia-friendly typeface (if enabled)
	bool dyslexia_font_loaded = false;
	if (ctx->dyslexia_font)
	{
		if (auto it = ctx->strings->find("font_dyslexia"); it != ctx->strings->end() && !it->second.empty() && it->second[0] != '#')
		{
			ctx->logger->log(it->second);
			ctx->typefaces["dyslexia"] = ctx->resource_manager->load<type::typeface>(it->second);
			dyslexia_font_loaded = true;
		}
	}
	
	// Load typefaces
	if (dyslexia_font_loaded)
	{
		// Override standard typefaces with dyslexia-friendly typeface
		ctx->typefaces["serif"] = ctx->typefaces["dyslexia"];
		ctx->typefaces["sans_serif"] = ctx->typefaces["dyslexia"];
		ctx->typefaces["monospace"] = ctx->typefaces["dyslexia"];
	}
	else
	{
		// Load standard typefaces
		if (auto it = ctx->strings->find("font_serif"); it != ctx->strings->end())
			ctx->typefaces["serif"] = ctx->resource_manager->load<type::typeface>(it->second);
		if (auto it = ctx->strings->find("font_sans_serif"); it != ctx->strings->end())
			ctx->typefaces["sans_serif"] = ctx->resource_manager->load<type::typeface>(it->second);
		if (auto it = ctx->strings->find("font_monospace"); it != ctx->strings->end())
			ctx->typefaces["monospace"] = ctx->resource_manager->load<type::typeface>(it->second);
	}
	
	// Build character set
	std::unordered_set<char32_t> charset;
	{
		// Add all character codes from the basic Latin unicode block
		for (char32_t code = type::unicode::block::basic_latin.first; code <= type::unicode::block::basic_latin.last; ++code)
			charset.insert(code);
		
		// Add all character codes from game strings
		for (auto it = ctx->strings->begin(); it != ctx->strings->end(); ++it)
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
	gl::shader_program* bitmap_font_shader = ctx->resource_manager->load<gl::shader_program>("bitmap-font.glsl");
	
	// Determine font point sizes
	float debug_font_size_pt = 12.0f;
	float menu_font_size_pt = 12.0f;
	float title_font_size_pt = 12.0f;
	if (ctx->config->contains("debug_font_size"))
		debug_font_size_pt = (*ctx->config)["debug_font_size"].get<float>();
	if (ctx->config->contains("menu_font_size"))
		menu_font_size_pt = (*ctx->config)["menu_font_size"].get<float>();
	if (ctx->config->contains("title_font_size"))
		title_font_size_pt = (*ctx->config)["title_font_size"].get<float>();

	// Scale font point sizes
	const float font_size = (*ctx->config)["font_size"].get<float>();
	debug_font_size_pt *= font_size;
	menu_font_size_pt *= font_size;
	title_font_size_pt *= font_size;
	
	// Convert font point sizes to pixel sizes
	const float dpi = ctx->app->get_display_dpi();
	const float debug_font_size_px = (debug_font_size_pt * dpi) / 72.0f;
	const float menu_font_size_px = (menu_font_size_pt * dpi) / 72.0f;
	const float title_font_size_px = (title_font_size_pt * dpi) / 72.0f;
	
	// Build debug font
	if (auto it = ctx->typefaces.find("monospace"); it != ctx->typefaces.end())
	{
		build_bitmap_font(*it->second, debug_font_size_px, charset, ctx->debug_font, ctx->debug_font_material, bitmap_font_shader);
	}
	
	// Build menu font
	if (auto it = ctx->typefaces.find("sans_serif"); it != ctx->typefaces.end())
	{
		build_bitmap_font(*it->second, menu_font_size_px, charset, ctx->menu_font, ctx->menu_font_material, bitmap_font_shader);
	}
	
	// Build title font
	if (auto it = ctx->typefaces.find("serif"); it != ctx->typefaces.end())
	{
		build_bitmap_font(*it->second, title_font_size_px, charset, ctx->title_font, ctx->title_font_material, bitmap_font_shader);
	}
}

} // namespace game

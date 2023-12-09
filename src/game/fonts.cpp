// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include "game/fonts.hpp"
#include <engine/type/type.hpp>
#include <engine/resources/resource-manager.hpp>
#include <engine/render/material.hpp>
#include <engine/render/material-flags.hpp>
#include <engine/hash/fnv1a.hpp>
#include "game/strings.hpp"
#include <codecvt>

static void build_bitmap_font(const type::typeface& typeface, float size, const std::unordered_set<char32_t>& charset, type::bitmap_font& font, render::material& material, std::shared_ptr<gl::shader_template> shader_template)
{
	// Get font metrics for given size
	if (type::font_metrics metrics; typeface.get_metrics(size, metrics))
	{
		font.set_font_metrics(metrics);
	}
	
	// For each UTF-32 character code in the character set
	for (char32_t code: charset)
	{
		// Skip missing glyphs
		if (!typeface.get_charset().contains(code))
		{
			continue;
		}
		
		// Add glyph to font
		type::bitmap_glyph& glyph = font.insert(code);
		typeface.get_metrics(size, code, glyph.metrics);
		typeface.get_bitmap(size, code, glyph.bitmap, glyph.bitmap_width, glyph.bitmap_height);
	}
	
	// Pack glyph bitmaps into the font bitmap
	font.pack();
	
	// Create font material
	material.set_blend_mode(render::material_blend_mode::translucent);
	
	// Create font bitmap variable
	if (auto var = material.get_variable("font_bitmap"); var)
	{
		std::static_pointer_cast<render::matvar_texture_2d>(var)->set(font.get_texture());
	}
	else
	{
		material.set_variable("font_bitmap", std::make_shared<render::matvar_texture_2d>(1, font.get_texture()));
	}
	
	material.set_shader_template(shader_template);
}

void load_fonts(::game& ctx)
{
	const auto& language = (*ctx.languages)[ctx.language_tag];
	
	// Load dyslexia-friendly typeface (if enabled)
	bool dyslexia_font_loaded = false;
	if (ctx.dyslexia_font)
	{
		const auto& dyslexia_font_path = language["font_dyslexia"];
		if (!dyslexia_font_path.is_null())
		{
			ctx.typefaces["dyslexia"] = ctx.resource_manager->load<type::typeface>(dyslexia_font_path);
			dyslexia_font_loaded = true;
		}
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
		const auto serif_font_path = language["font_serif"];
		const auto sans_serif_font_path = language["font_sans_serif"];
		const auto monospace_font_path = language["font_monospace"];
		
		ctx.typefaces["serif"] = ctx.resource_manager->load<type::typeface>(serif_font_path);
		ctx.typefaces["sans_serif"] = ctx.resource_manager->load<type::typeface>(sans_serif_font_path);
		ctx.typefaces["monospace"] = ctx.resource_manager->load<type::typeface>(monospace_font_path);
	}
	
	// Build character set
	/*
	std::unordered_set<char32_t> charset;
	{
		// Add all character codes from the basic Latin unicode block
		for (char32_t code = type::unicode::block::basic_latin.first; code <= type::unicode::block::basic_latin.last; ++code)
			charset.insert(code);
		
		// Add all character codes from game strings
		for (auto it = ctx.string_map->begin(); it != ctx.string_map->end(); ++it)
		{
			// Convert UTF-8 string to UTF-32
			std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> convert;
			std::u32string u32 = convert.from_bytes(it->second);
			
			/// Insert each character code from the UTF-32 string into the character set
			for (char32_t code: u32)
				charset.insert(code);
		}
	}
	*/
	
	// Load bitmap font shader
	std::shared_ptr<gl::shader_template> font_shader_template = ctx.resource_manager->load<gl::shader_template>("bitmap-font.glsl");
	
	// Point size to pixel size conversion factor
	const float dpi = ctx.window_manager->get_display(0).get_dpi();
	const float pt_to_px = (dpi / 72.0f) * ctx.font_scale;
	
	// Build debug font
	if (auto it = ctx.typefaces.find("monospace"); it != ctx.typefaces.end())
	{
		build_bitmap_font(*it->second, ctx.debug_font_size_pt * pt_to_px, it->second->get_charset(), ctx.debug_font, *ctx.debug_font_material, font_shader_template);
	}
	
	// Build menu font
	if (auto it = ctx.typefaces.find("sans_serif"); it != ctx.typefaces.end())
	{
		build_bitmap_font(*it->second, ctx.menu_font_size_pt * pt_to_px, it->second->get_charset(), ctx.menu_font, *ctx.menu_font_material, font_shader_template);
	}
	
	// Build title font
	if (auto it = ctx.typefaces.find("serif"); it != ctx.typefaces.end())
	{
		build_bitmap_font(*it->second, ctx.title_font_size_pt * pt_to_px, it->second->get_charset(), ctx.title_font, *ctx.title_font_material, font_shader_template);
	}
}


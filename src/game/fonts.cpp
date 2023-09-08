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
#include <engine/type/type.hpp>
#include <engine/resources/resource-manager.hpp>
#include <engine/gl/texture-wrapping.hpp>
#include <engine/gl/texture-filter.hpp>
#include <engine/render/material.hpp>
#include <engine/render/material-flags.hpp>
#include <engine/utility/hash/fnv1a.hpp>
#include "game/strings.hpp"
#include <codecvt>

static void build_bitmap_font(const type::typeface& typeface, float size, const std::unordered_set<char32_t>& charset, type::bitmap_font& font, render::material& material, std::shared_ptr<gl::shader_template> shader_template)
{
	// Get font metrics for given size
	if (type::font_metrics metrics; typeface.get_metrics(size, metrics))
	{
		font.set_font_metrics(metrics);
	}
	
	// Format font bitmap
	image& font_bitmap = font.get_bitmap();
	font_bitmap.format(1, sizeof(std::byte) * 8);
	
	// For each UTF-32 character code in the character set
	for (char32_t code: charset)
	{
		// Skip missing glyphs
		if (!typeface.get_charset().contains(code))
		{
			continue;
		}
		
		// Add glyph to font
		type::bitmap_glyph& glyph = font[code];
		typeface.get_metrics(size, code, glyph.metrics);
		typeface.get_bitmap(size, code, glyph.bitmap);
	}
	
	// Pack glyph bitmaps into the font bitmap
	font.pack();
	
	// Create font material
	material.set_blend_mode(render::material_blend_mode::translucent);
	if (auto var = material.get_variable("font_bitmap"))
	{
		// Update font texture
		auto texture = std::static_pointer_cast<render::matvar_texture_2d>(var)->get();
		texture->resize(static_cast<std::uint16_t>(font_bitmap.size().x()), static_cast<std::uint16_t>(font_bitmap.size().y()), font_bitmap.data());
	}
	else
	{
		// Create font texture from bitmap
		std::shared_ptr<gl::texture_2d> font_texture = std::make_shared<gl::texture_2d>(static_cast<std::uint16_t>(font_bitmap.size().x()), static_cast<std::uint16_t>(font_bitmap.size().y()), gl::pixel_type::uint_8, gl::pixel_format::r, gl::transfer_function::linear, font_bitmap.data());
		font_texture->set_wrapping(gl::texture_wrapping::extend, gl::texture_wrapping::extend);
		font_texture->set_filters(gl::texture_min_filter::linear, gl::texture_mag_filter::linear);
		
		// Create font bitmap variable
		material.set_variable("font_bitmap", std::make_shared<render::matvar_texture_2d>(1, font_texture));
	}
	material.set_shader_template(shader_template);
}

void load_fonts(::game& ctx)
{
	// Load dyslexia-friendly typeface (if enabled)
	bool dyslexia_font_loaded = false;
	if (ctx.dyslexia_font)
	{
		const auto dyslexia_font_path = get_string(ctx, "font_dyslexia");
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
		const auto serif_font_path = get_string(ctx, "font_serif");
		const auto sans_serif_font_path = get_string(ctx, "font_sans_serif");
		const auto monospace_font_path = get_string(ctx, "font_monospace");
		
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


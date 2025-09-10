// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <nlohmann/json.hpp>
#include "game/fonts.hpp"
#include "game/strings.hpp"
#include <engine/hash/fnv.hpp>
#include <engine/type/font.hpp>
#include <engine/resources/resource-manager.hpp>
#include <engine/render/material.hpp>
#include <engine/gl/shader-template.hpp>

using namespace engine;
using namespace engine::hash::literals;

void build_font_material(render::material& material, const type::font& font, std::shared_ptr<gl::shader_template> shader_template)
{
	// Set font material blend mode
	material.set_blend_mode(render::material_blend_mode::translucent);
	
	// Set font material shader template
	material.set_shader_template(shader_template);
	
	// Set font material bitmap variable
	if (auto var = material.get_variable("font_bitmap"_fnv1a32))
	{
		std::static_pointer_cast<render::matvar_texture_2d>(var)->set(font.get_texture());
	}
	else
	{
		material.set_variable("font_bitmap"_fnv1a32, std::make_shared<render::matvar_texture_2d>(1, font.get_texture()));
	}
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
			ctx.typefaces["dyslexia"_fnv1a32] = ctx.resource_manager->load<type::typeface>(dyslexia_font_path);
			dyslexia_font_loaded = true;
		}
	}
	
	// Load typefaces
	if (dyslexia_font_loaded)
	{
		// Override standard typefaces with dyslexia-friendly typeface
		ctx.typefaces["serif"_fnv1a32] = ctx.typefaces["dyslexia"_fnv1a32];
		ctx.typefaces["sans_serif"_fnv1a32] = ctx.typefaces["dyslexia"_fnv1a32];
		ctx.typefaces["monospace"_fnv1a32] = ctx.typefaces["dyslexia"_fnv1a32];
	}
	else
	{
		// Load standard typefaces
		const auto serif_font_path = language["font_serif"];
		const auto sans_serif_font_path = language["font_sans_serif"];
		const auto monospace_font_path = language["font_monospace"];
		
		ctx.typefaces["serif"_fnv1a32] = ctx.resource_manager->load<type::typeface>(serif_font_path);
		ctx.typefaces["sans_serif"_fnv1a32] = ctx.resource_manager->load<type::typeface>(sans_serif_font_path);
		ctx.typefaces["monospace"_fnv1a32] = ctx.resource_manager->load<type::typeface>(monospace_font_path);
	}
	
	// Load bitmap font shader
	std::shared_ptr<gl::shader_template> font_shader_template = ctx.resource_manager->load<gl::shader_template>("bitmap-font.glsl");
	
	// Point size to pixel size conversion factor
	const float dpi = ctx.window->get_display_scale() * 96.0f;
	const float pt_to_px = (dpi / 72.0f) * ctx.font_scale;
	
	// Build debug font
	if (auto it = ctx.typefaces.find("monospace"_fnv1a32); it != ctx.typefaces.end())
	{
		ctx.debug_font = std::make_shared<type::font>(it->second, ctx.debug_font_size_pt * pt_to_px);
		build_font_material(*ctx.debug_font_material, *ctx.debug_font, font_shader_template);
	}
	
	// Build menu font
	if (auto it = ctx.typefaces.find("sans_serif"_fnv1a32); it != ctx.typefaces.end())
	{
		ctx.menu_font = std::make_shared<type::font>(it->second, ctx.menu_font_size_pt * pt_to_px);
		build_font_material(*ctx.menu_font_material, *ctx.menu_font, font_shader_template);
	}
	
	// Build title font
	if (auto it = ctx.typefaces.find("serif"_fnv1a32); it != ctx.typefaces.end())
	{
		ctx.title_font = std::make_shared<type::font>(it->second, ctx.title_font_size_pt * pt_to_px);
		build_font_material(*ctx.title_font_material, *ctx.title_font, font_shader_template);
	}
}

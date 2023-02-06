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

#include "game/state/graphics-menu.hpp"
#include "game/state/options-menu.hpp"
#include "scene/text.hpp"
#include "debug/log.hpp"
#include "game/fonts.hpp"
#include "game/menu.hpp"
#include "game/graphics.hpp"
#include "animation/timeline.hpp"
#include "game/strings.hpp"
#include "utility/hash/fnv1a.hpp"

using namespace hash::literals;

namespace game {
namespace state {

static void update_value_text_content(game::context* ctx);

graphics_menu::graphics_menu(game::context& ctx):
	game::state::base(ctx)
{
	debug::log::trace("Entering graphics menu state...");
	
	// Construct menu item texts
	scene::text* fullscreen_name_text = new scene::text();
	scene::text* fullscreen_value_text = new scene::text();
	scene::text* resolution_name_text = new scene::text();
	scene::text* resolution_value_text = new scene::text();
	scene::text* v_sync_name_text = new scene::text();
	scene::text* v_sync_value_text = new scene::text();
	scene::text* aa_method_name_text = new scene::text();
	scene::text* aa_method_value_text = new scene::text();
	scene::text* font_scale_name_text = new scene::text();
	scene::text* font_scale_value_text = new scene::text();
	scene::text* dyslexia_font_name_text = new scene::text();
	scene::text* dyslexia_font_value_text = new scene::text();
	scene::text* back_text = new scene::text();
	
	// Build list of menu item texts
	ctx.menu_item_texts.push_back({fullscreen_name_text, fullscreen_value_text});
	ctx.menu_item_texts.push_back({resolution_name_text, resolution_value_text});
	ctx.menu_item_texts.push_back({v_sync_name_text, v_sync_value_text});
	ctx.menu_item_texts.push_back({aa_method_name_text, aa_method_value_text});
	ctx.menu_item_texts.push_back({font_scale_name_text, font_scale_value_text});
	ctx.menu_item_texts.push_back({dyslexia_font_name_text, dyslexia_font_value_text});
	ctx.menu_item_texts.push_back({back_text, nullptr});
	
	// Set content of menu item texts
	fullscreen_name_text->set_content(get_string(ctx, "graphics_menu_fullscreen"_fnv1a32));
	resolution_name_text->set_content(get_string(ctx, "graphics_menu_resolution"_fnv1a32));
	v_sync_name_text->set_content(get_string(ctx, "graphics_menu_v_sync"_fnv1a32));
	aa_method_name_text->set_content(get_string(ctx, "graphics_menu_aa_method"_fnv1a32));
	font_scale_name_text->set_content(get_string(ctx, "graphics_menu_font_scale"_fnv1a32));
	dyslexia_font_name_text->set_content(get_string(ctx, "graphics_menu_dyslexia_font"_fnv1a32));
	back_text->set_content(get_string(ctx, "back"_fnv1a32));
	update_value_text_content();
	
	// Init menu item index
	game::menu::init_menu_item_index(ctx, "graphics");
	
	game::menu::update_text_color(ctx);
	game::menu::update_text_font(ctx);
	game::menu::align_text(ctx);
	game::menu::update_text_tweens(ctx);
	game::menu::add_text_to_ui(ctx);
	game::menu::setup_animations(ctx);
	
	// Construct menu item callbacks
	auto toggle_fullscreen_callback = [this, &ctx]()
	{
		bool fullscreen = !ctx.window->is_fullscreen();
		
		ctx.window->set_fullscreen(fullscreen);
		
		
		this->update_value_text_content();
		game::menu::align_text(ctx);
		game::menu::update_text_tweens(ctx);
		
		// Update fullscreen settings
		(*ctx.settings)["fullscreen"_fnv1a32] = fullscreen;
	};
	
	auto increase_resolution_callback = [this, &ctx]()
	{
		// Increase resolution
		if (ctx.menu_modifier_control.is_active())
			ctx.render_scale += 0.05f;
		else
			ctx.render_scale += 0.25f;
		
		// Limit resolution
		if (ctx.render_scale > 2.0f)
			ctx.render_scale = 2.0f;
		
		// Update render scale setting
		(*ctx.settings)["render_scale"_fnv1a32] = ctx.render_scale;
		
		// Resize framebuffers
		game::graphics::change_render_resolution(ctx, ctx.render_scale);
		
		// Update text
		this->update_value_text_content();
		game::menu::align_text(ctx);
		game::menu::update_text_tweens(ctx);
	};
	
	auto decrease_resolution_callback = [this, &ctx]()
	{
		// Increase resolution
		if (ctx.menu_modifier_control.is_active())
			ctx.render_scale -= 0.05f;
		else
			ctx.render_scale -= 0.25f;
		
		// Limit resolution
		if (ctx.render_scale < 0.25f)
			ctx.render_scale = 0.25f;
		
		// Update render scale setting
		(*ctx.settings)["render_scale"_fnv1a32] = ctx.render_scale;
		
		// Resize framebuffers
		game::graphics::change_render_resolution(ctx, ctx.render_scale);
		
		// Update text
		this->update_value_text_content();
		game::menu::align_text(ctx);
		game::menu::update_text_tweens(ctx);
	};
	
	auto toggle_v_sync_callback = [this, &ctx]()
	{
		bool v_sync = !ctx.window->get_v_sync();
		
		// Update v-sync setting
		(*ctx.settings)["v_sync"_fnv1a32] = v_sync;
		
		ctx.window->set_v_sync(v_sync);
		
		this->update_value_text_content();
		game::menu::align_text(ctx);
		game::menu::update_text_tweens(ctx);
	};
	
	auto next_aa_method_callback = [this, &ctx]()
	{
		switch (ctx.anti_aliasing_method)
		{
			case render::anti_aliasing_method::none:
				ctx.anti_aliasing_method = render::anti_aliasing_method::fxaa;
				break;
			
			case render::anti_aliasing_method::fxaa:
				ctx.anti_aliasing_method = render::anti_aliasing_method::none;
				break;
		}
		
		// Update anti-aliasing method setting
		(*ctx.settings)["anti_aliasing_method"_fnv1a32] = std::to_underlying(ctx.anti_aliasing_method);
		
		game::graphics::select_anti_aliasing_method(ctx, ctx.anti_aliasing_method);
		
		// Update value text
		this->update_value_text_content();
		
		// Refresh and realign text
		game::menu::refresh_text(ctx);
		game::menu::align_text(ctx);
		game::menu::update_text_tweens(ctx);
	};
	
	auto previous_aa_method_callback = [this, &ctx]()
	{
		switch (ctx.anti_aliasing_method)
		{
			case render::anti_aliasing_method::none:
				ctx.anti_aliasing_method = render::anti_aliasing_method::fxaa;
				break;
			
			case render::anti_aliasing_method::fxaa:
				ctx.anti_aliasing_method = render::anti_aliasing_method::none;
				break;
		}
		
		// Update anti-aliasing method setting
		(*ctx.settings)["anti_aliasing_method"_fnv1a32] = std::to_underlying(ctx.anti_aliasing_method);
		
		game::graphics::select_anti_aliasing_method(ctx, ctx.anti_aliasing_method);
		
		// Update value text
		this->update_value_text_content();
		
		// Refresh and realign text
		game::menu::refresh_text(ctx);
		game::menu::align_text(ctx);
		game::menu::update_text_tweens(ctx);
	};
	
	auto increase_font_scale_callback = [this, &ctx]()
	{
		// Increase font scale
		if (ctx.menu_modifier_control.is_active())
			ctx.font_scale += 0.01f;
		else
			ctx.font_scale += 0.1f;
		
		// Limit font scale
		if (ctx.font_scale > 2.0f)
			ctx.font_scale = 2.0f;
		
		// Update font scale setting
		(*ctx.settings)["font_scale"_fnv1a32] = ctx.font_scale;
		
		// Update value text
		this->update_value_text_content();
		
		// Reload fonts
		debug::log::trace("Reloading fonts...");
		game::load_fonts(ctx);
		debug::log::trace("Reloaded fonts");
		
		// Refresh and realign text
		game::menu::refresh_text(ctx);
		game::menu::align_text(ctx);
		game::menu::update_text_tweens(ctx);
	};
	
	auto decrease_font_scale_callback = [this, &ctx]()
	{
		// Decrease font scale
		if (ctx.menu_modifier_control.is_active())
			ctx.font_scale -= 0.01f;
		else
			ctx.font_scale -= 0.1f;
		
		// Limit font scale
		if (ctx.font_scale < 0.1f)
			ctx.font_scale = 0.1f;
		
		// Update font scale setting
		(*ctx.settings)["font_scale"_fnv1a32] = ctx.font_scale;
		
		// Update value text
		this->update_value_text_content();
		
		// Reload fonts
		debug::log::trace("Reloading fonts...");
		game::load_fonts(ctx);
		debug::log::trace("Reloaded fonts");
		
		// Refresh and realign text
		game::menu::refresh_text(ctx);
		game::menu::align_text(ctx);
		game::menu::update_text_tweens(ctx);
	};
	
	auto toggle_dyslexia_font_callback = [this, &ctx]()
	{
		ctx.dyslexia_font = !ctx.dyslexia_font;
		
		// Update value text
		this->update_value_text_content();
		
		// Save dyslexia font setting
		(*ctx.settings)["dyslexia_font"_fnv1a32] = ctx.dyslexia_font;
		
		// Reload fonts
		debug::log::trace("Reloading fonts...");
		game::load_fonts(ctx);
		debug::log::trace("Reloaded fonts");
		
		// Refresh and realign text
		game::menu::refresh_text(ctx);
		game::menu::align_text(ctx);
		game::menu::update_text_tweens(ctx);
	};
	auto select_back_callback = [&ctx]()
	{
		// Disable controls
		game::menu::clear_controls(ctx);
		
		game::menu::fade_out
		(
			ctx,
			[&ctx]()
			{
				// Queue change to options menu state
				ctx.function_queue.push
				(
					[&ctx]()
					{
						ctx.state_machine.pop();
						ctx.state_machine.emplace(new game::state::options_menu(ctx));
					}
				);
			}
		);
	};
	
	// Build list of menu select callbacks
	ctx.menu_select_callbacks.push_back(toggle_fullscreen_callback);
	ctx.menu_select_callbacks.push_back(increase_resolution_callback);
	ctx.menu_select_callbacks.push_back(toggle_v_sync_callback);
	ctx.menu_select_callbacks.push_back(next_aa_method_callback);
	ctx.menu_select_callbacks.push_back(increase_font_scale_callback);
	ctx.menu_select_callbacks.push_back(toggle_dyslexia_font_callback);
	ctx.menu_select_callbacks.push_back(select_back_callback);
	
	// Build list of menu left callbacks
	ctx.menu_left_callbacks.push_back(toggle_fullscreen_callback);
	ctx.menu_left_callbacks.push_back(decrease_resolution_callback);
	ctx.menu_left_callbacks.push_back(toggle_v_sync_callback);
	ctx.menu_left_callbacks.push_back(previous_aa_method_callback);
	ctx.menu_left_callbacks.push_back(decrease_font_scale_callback);
	ctx.menu_left_callbacks.push_back(toggle_dyslexia_font_callback);
	ctx.menu_left_callbacks.push_back(nullptr);
	
	// Build list of menu right callbacks
	ctx.menu_right_callbacks.push_back(toggle_fullscreen_callback);
	ctx.menu_right_callbacks.push_back(increase_resolution_callback);
	ctx.menu_right_callbacks.push_back(toggle_v_sync_callback);
	ctx.menu_right_callbacks.push_back(next_aa_method_callback);
	ctx.menu_right_callbacks.push_back(increase_font_scale_callback);
	ctx.menu_right_callbacks.push_back(toggle_dyslexia_font_callback);
	ctx.menu_right_callbacks.push_back(nullptr);
	
	// Set menu back callback
	ctx.menu_back_callback = select_back_callback;
	
	// Queue menu control setup
	ctx.function_queue.push(std::bind(game::menu::setup_controls, std::ref(ctx)));
	
	// Fade in menu
	game::menu::fade_in(ctx, nullptr);
	
	debug::log::trace("Entered graphics menu state");
}

graphics_menu::~graphics_menu()
{
	debug::log::trace("Exiting graphics menu state...");
	
	// Destruct menu
	game::menu::clear_controls(ctx);
	game::menu::clear_callbacks(ctx);
	game::menu::delete_animations(ctx);
	game::menu::remove_text_from_ui(ctx);
	game::menu::delete_text(ctx);
	
	debug::log::trace("Exited graphics menu state");
}

void graphics_menu::update_value_text_content()
{
	const bool fullscreen = ctx.window->is_fullscreen();
	const float render_scale = ctx.render_scale;
	const bool v_sync = ctx.window->get_v_sync();
	const int aa_method_index = static_cast<int>(ctx.anti_aliasing_method);
	const float font_scale = ctx.font_scale;
	const bool dyslexia_font = ctx.dyslexia_font;
	
	const std::string string_on = get_string(ctx, "on"_fnv1a32);
	const std::string string_off = get_string(ctx, "off"_fnv1a32);
	
	/*
	const std::string string_quality[4] = 
	{
		(*ctx.strings)["off"],
		(*ctx.strings)["quality_low"],
		(*ctx.strings)["quality_medium"],
		(*ctx.strings)["quality_high"]
	};
	*/
	
	const std::string string_aa_methods[2] = 
	{
		get_string(ctx, "graphics_menu_aa_method_none"_fnv1a32),
		get_string(ctx, "graphics_menu_aa_method_fxaa"_fnv1a32)
	};
	
	std::get<1>(ctx.menu_item_texts[0])->set_content((fullscreen) ? string_on : string_off);
	std::get<1>(ctx.menu_item_texts[1])->set_content(std::to_string(static_cast<int>(std::round(render_scale * 100.0f))) + "%");
	std::get<1>(ctx.menu_item_texts[2])->set_content((v_sync) ? string_on : string_off);
	std::get<1>(ctx.menu_item_texts[3])->set_content(string_aa_methods[aa_method_index]);
	std::get<1>(ctx.menu_item_texts[5])->set_content(std::to_string(static_cast<int>(std::round(font_scale * 100.0f))) + "%");
	std::get<1>(ctx.menu_item_texts[6])->set_content((dyslexia_font) ? string_on : string_off);
}

} // namespace state
} // namespace game

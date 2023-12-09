// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include "game/states/graphics-menu-state.hpp"
#include "game/states/options-menu-state.hpp"
#include "game/controls.hpp"
#include <engine/scene/text.hpp>
#include <engine/debug/log.hpp>
#include "game/fonts.hpp"
#include "game/menu.hpp"
#include "game/graphics.hpp"
#include <engine/animation/timeline.hpp>
#include "game/strings.hpp"
#include <engine/hash/fnv1a.hpp>

using namespace hash::literals;


graphics_menu_state::graphics_menu_state(::game& ctx):
	game_state(ctx)
{
	debug::log_trace("Entering graphics menu state...");
	
	// Construct menu item texts
	fullscreen_name_text = std::make_unique<scene::text>();
	fullscreen_value_text = std::make_unique<scene::text>();
	resolution_name_text = std::make_unique<scene::text>();
	resolution_value_text = std::make_unique<scene::text>();
	v_sync_name_text = std::make_unique<scene::text>();
	v_sync_value_text = std::make_unique<scene::text>();
	aa_method_name_text = std::make_unique<scene::text>();
	aa_method_value_text = std::make_unique<scene::text>();
	font_scale_name_text = std::make_unique<scene::text>();
	font_scale_value_text = std::make_unique<scene::text>();
	dyslexia_font_name_text = std::make_unique<scene::text>();
	dyslexia_font_value_text = std::make_unique<scene::text>();
	back_text = std::make_unique<scene::text>();
	
	// Build list of menu item texts
	ctx.menu_item_texts.push_back({fullscreen_name_text.get(), fullscreen_value_text.get()});
	ctx.menu_item_texts.push_back({resolution_name_text.get(), resolution_value_text.get()});
	ctx.menu_item_texts.push_back({v_sync_name_text.get(), v_sync_value_text.get()});
	ctx.menu_item_texts.push_back({aa_method_name_text.get(), aa_method_value_text.get()});
	ctx.menu_item_texts.push_back({font_scale_name_text.get(), font_scale_value_text.get()});
	ctx.menu_item_texts.push_back({dyslexia_font_name_text.get(), dyslexia_font_value_text.get()});
	ctx.menu_item_texts.push_back({back_text.get(), nullptr});
	
	// Set content of menu item texts
	fullscreen_name_text->set_content(get_string(ctx, "graphics_menu_fullscreen"));
	resolution_name_text->set_content(get_string(ctx, "graphics_menu_resolution"));
	v_sync_name_text->set_content(get_string(ctx, "graphics_menu_v_sync"));
	aa_method_name_text->set_content(get_string(ctx, "graphics_menu_aa_method"));
	font_scale_name_text->set_content(get_string(ctx, "graphics_menu_font_scale"));
	dyslexia_font_name_text->set_content(get_string(ctx, "graphics_menu_dyslexia_font"));
	back_text->set_content(get_string(ctx, "back"));
	update_value_text_content();
	
	// Init menu item index
	::menu::init_menu_item_index(ctx, "graphics");
	
	::menu::update_text_color(ctx);
	::menu::update_text_font(ctx);
	::menu::align_text(ctx);
	::menu::add_text_to_ui(ctx);
	::menu::setup_animations(ctx);
	
	// Construct menu item callbacks
	auto toggle_fullscreen_callback = [this, &ctx]()
	{
		bool fullscreen = !ctx.window->is_fullscreen();
		
		ctx.window->set_fullscreen(fullscreen);
		
		this->update_value_text_content();
		::menu::align_text(ctx);
		
		// Update fullscreen settings
		(*ctx.settings)["fullscreen"] = fullscreen;
	};
	
	auto increase_resolution_callback = [this, &ctx]()
	{
		// Increase resolution
		if (ctx.menu_modifier_action.is_active())
			ctx.render_scale += 0.05f;
		else
			ctx.render_scale += 0.25f;
		
		// Limit resolution
		if (ctx.render_scale > 2.0f)
			ctx.render_scale = 2.0f;
		
		// Update render scale setting
		(*ctx.settings)["render_scale"] = ctx.render_scale;
		
		// Resize framebuffers
		::graphics::change_render_resolution(ctx, ctx.render_scale);
		
		// Update text
		this->update_value_text_content();
		::menu::align_text(ctx);
	};
	
	auto decrease_resolution_callback = [this, &ctx]()
	{
		// Increase resolution
		if (ctx.menu_modifier_action.is_active())
			ctx.render_scale -= 0.05f;
		else
			ctx.render_scale -= 0.25f;
		
		// Limit resolution
		if (ctx.render_scale < 0.25f)
			ctx.render_scale = 0.25f;
		
		// Update render scale setting
		(*ctx.settings)["render_scale"] = ctx.render_scale;
		
		// Resize framebuffers
		::graphics::change_render_resolution(ctx, ctx.render_scale);
		
		// Update text
		this->update_value_text_content();
		::menu::align_text(ctx);
	};
	
	auto toggle_v_sync_callback = [this, &ctx]()
	{
		bool v_sync = !ctx.window->get_v_sync();
		
		// Update v-sync setting
		(*ctx.settings)["v_sync"] = v_sync;
		
		ctx.window->set_v_sync(v_sync);
		
		this->update_value_text_content();
		::menu::align_text(ctx);
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
		(*ctx.settings)["anti_aliasing_method"] = std::to_underlying(ctx.anti_aliasing_method);
		
		::graphics::select_anti_aliasing_method(ctx, ctx.anti_aliasing_method);
		
		// Update value text
		this->update_value_text_content();
		
		// Refresh and realign text
		::menu::refresh_text(ctx);
		::menu::align_text(ctx);
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
		(*ctx.settings)["anti_aliasing_method"] = std::to_underlying(ctx.anti_aliasing_method);
		
		::graphics::select_anti_aliasing_method(ctx, ctx.anti_aliasing_method);
		
		// Update value text
		this->update_value_text_content();
		
		// Refresh and realign text
		::menu::refresh_text(ctx);
		::menu::align_text(ctx);
	};
	
	auto increase_font_scale_callback = [this, &ctx]()
	{
		// Increase font scale
		if (ctx.menu_modifier_action.is_active())
			ctx.font_scale += 0.01f;
		else
			ctx.font_scale += 0.1f;
		
		// Limit font scale
		if (ctx.font_scale > 2.0f)
			ctx.font_scale = 2.0f;
		
		// Update font scale setting
		(*ctx.settings)["font_scale"] = ctx.font_scale;
		
		// Update value text
		this->update_value_text_content();
		
		// Reload fonts
		debug::log_trace("Reloading fonts...");
		::load_fonts(ctx);
		debug::log_trace("Reloaded fonts");
		
		// Refresh and realign text
		::menu::refresh_text(ctx);
		::menu::align_text(ctx);
	};
	
	auto decrease_font_scale_callback = [this, &ctx]()
	{
		// Decrease font scale
		if (ctx.menu_modifier_action.is_active())
			ctx.font_scale -= 0.01f;
		else
			ctx.font_scale -= 0.1f;
		
		// Limit font scale
		if (ctx.font_scale < 0.1f)
			ctx.font_scale = 0.1f;
		
		// Update font scale setting
		(*ctx.settings)["font_scale"] = ctx.font_scale;
		
		// Update value text
		this->update_value_text_content();
		
		// Reload fonts
		debug::log_trace("Reloading fonts...");
		::load_fonts(ctx);
		debug::log_trace("Reloaded fonts");
		
		// Refresh and realign text
		::menu::refresh_text(ctx);
		::menu::align_text(ctx);
	};
	
	auto toggle_dyslexia_font_callback = [this, &ctx]()
	{
		ctx.dyslexia_font = !ctx.dyslexia_font;
		
		// Update value text
		this->update_value_text_content();
		
		// Save dyslexia font setting
		(*ctx.settings)["dyslexia_font"] = ctx.dyslexia_font;
		
		// Reload fonts
		debug::log_trace("Reloading fonts...");
		::load_fonts(ctx);
		debug::log_trace("Reloaded fonts");
		
		// Refresh and realign text
		::menu::refresh_text(ctx);
		::menu::align_text(ctx);
	};
	auto select_back_callback = [&ctx]()
	{
		// Disable menu controls
		ctx.function_queue.push(std::bind(::disable_menu_controls, std::ref(ctx)));
		
		::menu::fade_out
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
						ctx.state_machine.emplace(std::make_unique<options_menu_state>(ctx));
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
	
	// Enable menu controls next frame
	ctx.function_queue.push(std::bind(::enable_menu_controls, std::ref(ctx)));
	
	// Fade in menu
	::menu::fade_in(ctx, nullptr);
	
	debug::log_trace("Entered graphics menu state");
}

graphics_menu_state::~graphics_menu_state()
{
	debug::log_trace("Exiting graphics menu state...");
	
	// Destruct menu
	::disable_menu_controls(ctx);
	::menu::clear_callbacks(ctx);
	::menu::delete_animations(ctx);
	::menu::remove_text_from_ui(ctx);
	::menu::delete_text(ctx);
	
	debug::log_trace("Exited graphics menu state");
}

void graphics_menu_state::update_value_text_content()
{
	const bool fullscreen = ctx.window->is_fullscreen();
	const float render_scale = ctx.render_scale;
	const bool v_sync = ctx.window->get_v_sync();
	const int aa_method_index = static_cast<int>(ctx.anti_aliasing_method);
	const float font_scale = ctx.font_scale;
	const bool dyslexia_font = ctx.dyslexia_font;
	
	const std::string string_on = get_string(ctx, "on");
	const std::string string_off = get_string(ctx, "off");
	
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
		get_string(ctx, "graphics_menu_aa_method_none"),
		get_string(ctx, "graphics_menu_aa_method_fxaa")
	};
	
	std::get<1>(ctx.menu_item_texts[0])->set_content((fullscreen) ? string_on : string_off);
	std::get<1>(ctx.menu_item_texts[1])->set_content(std::to_string(static_cast<int>(std::round(render_scale * 100.0f))) + "%");
	std::get<1>(ctx.menu_item_texts[2])->set_content((v_sync) ? string_on : string_off);
	std::get<1>(ctx.menu_item_texts[3])->set_content(string_aa_methods[aa_method_index]);
	std::get<1>(ctx.menu_item_texts[4])->set_content(std::to_string(static_cast<int>(std::round(font_scale * 100.0f))) + "%");
	std::get<1>(ctx.menu_item_texts[5])->set_content((dyslexia_font) ? string_on : string_off);
}

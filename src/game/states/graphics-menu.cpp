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

#include "game/states/graphics-menu.hpp"
#include "game/states/options-menu.hpp"
#include "application.hpp"
#include "scene/text.hpp"
#include "debug/logger.hpp"
#include "game/fonts.hpp"
#include "game/menu.hpp"
#include "animation/timeline.hpp"

namespace game {
namespace state {
namespace graphics_menu {

static void update_value_text_content(game::context* ctx)
{
	bool fullscreen = ctx->app->is_fullscreen();
	float resolution = ctx->render_resolution_scale;
	bool v_sync = ctx->app->get_v_sync();
	float font_size = ctx->font_size;
	bool dyslexia_font = ctx->dyslexia_font;
	
	const std::string string_on = (*ctx->strings)["on"];
	const std::string string_off = (*ctx->strings)["off"];
	
	std::get<1>(ctx->menu_item_texts[0])->set_content((fullscreen) ? string_on : string_off);
	std::get<1>(ctx->menu_item_texts[1])->set_content(std::to_string(static_cast<int>(std::round(resolution * 100.0f))) + "%");
	std::get<1>(ctx->menu_item_texts[2])->set_content((v_sync) ? string_on : string_off);
	std::get<1>(ctx->menu_item_texts[3])->set_content(std::to_string(static_cast<int>(std::round(font_size * 100.0f))) + "%");
	std::get<1>(ctx->menu_item_texts[4])->set_content((dyslexia_font) ? string_on : string_off);
}

void enter(game::context* ctx)
{
	// Construct menu item texts
	scene::text* fullscreen_name_text = new scene::text();
	scene::text* fullscreen_value_text = new scene::text();
	scene::text* resolution_name_text = new scene::text();
	scene::text* resolution_value_text = new scene::text();
	scene::text* v_sync_name_text = new scene::text();
	scene::text* v_sync_value_text = new scene::text();
	scene::text* font_size_name_text = new scene::text();
	scene::text* font_size_value_text = new scene::text();
	scene::text* dyslexia_font_name_text = new scene::text();
	scene::text* dyslexia_font_value_text = new scene::text();
	scene::text* back_text = new scene::text();
	
	// Build list of menu item texts
	ctx->menu_item_texts.push_back({fullscreen_name_text, fullscreen_value_text});
	ctx->menu_item_texts.push_back({resolution_name_text, resolution_value_text});
	ctx->menu_item_texts.push_back({v_sync_name_text, v_sync_value_text});
	ctx->menu_item_texts.push_back({font_size_name_text, font_size_value_text});
	ctx->menu_item_texts.push_back({dyslexia_font_name_text, dyslexia_font_value_text});
	ctx->menu_item_texts.push_back({back_text, nullptr});
	
	// Set content of menu item texts
	fullscreen_name_text->set_content((*ctx->strings)["graphics_menu_fullscreen"]);
	resolution_name_text->set_content((*ctx->strings)["graphics_menu_resolution"]);
	v_sync_name_text->set_content((*ctx->strings)["graphics_menu_v_sync"]);
	font_size_name_text->set_content((*ctx->strings)["graphics_menu_font_size"]);
	dyslexia_font_name_text->set_content((*ctx->strings)["graphics_menu_dyslexia_font"]);
	back_text->set_content((*ctx->strings)["back"]);
	update_value_text_content(ctx);
	
	// Init menu item index
	game::menu::init_menu_item_index(ctx, "graphics");
	
	game::menu::update_text_color(ctx);
	game::menu::update_text_font(ctx);
	game::menu::align_text(ctx);
	game::menu::update_text_tweens(ctx);
	game::menu::add_text_to_ui(ctx);
	game::menu::setup_animations(ctx);
	
	// Construct menu item callbacks
	auto toggle_fullscreen_callback = [ctx]()
	{
		bool fullscreen = !ctx->app->is_fullscreen();
		
		ctx->app->set_fullscreen(fullscreen);
		
		if (!fullscreen)
		{
			int2 resolution;
			resolution.x = (*ctx->config)["windowed_resolution"][0].get<int>();
			resolution.y = (*ctx->config)["windowed_resolution"][1].get<int>();
			
			ctx->app->resize_window(resolution.x, resolution.y);
		}
		
		update_value_text_content(ctx);
		game::menu::align_text(ctx);
		game::menu::update_text_tweens(ctx);
		
		// Save display mode config
		(*ctx->config)["fullscreen"] = fullscreen;
	};
	
	auto increase_resolution_callback = [ctx]()
	{
		// Increase resolution
		if (ctx->controls["menu_modifier"]->is_active())
			ctx->render_resolution_scale += 0.01f;
		else
			ctx->render_resolution_scale += 0.1f;
		
		// Limit resolution
		if (ctx->render_resolution_scale > 2.0f)
			ctx->render_resolution_scale = 2.0f;
		
		update_value_text_content(ctx);
		game::menu::align_text(ctx);
		game::menu::update_text_tweens(ctx);
		
		// Update config
		(*ctx->config)["render_resolution"] = ctx->render_resolution_scale;
	};
	
	auto decrease_resolution_callback = [ctx]()
	{
		// Increase resolution
		if (ctx->controls["menu_modifier"]->is_active())
			ctx->render_resolution_scale -= 0.01f;
		else
			ctx->render_resolution_scale -= 0.1f;
		
		// Limit resolution
		if (ctx->render_resolution_scale < 0.1f)
			ctx->render_resolution_scale = 0.1f;
		
		update_value_text_content(ctx);
		game::menu::align_text(ctx);
		game::menu::update_text_tweens(ctx);
		
		// Update config
		(*ctx->config)["render_resolution"] = ctx->render_resolution_scale;
	};
	
	auto toggle_v_sync_callback = [ctx]()
	{
		bool v_sync = !ctx->app->get_v_sync();
		
		ctx->app->set_v_sync(v_sync);
		
		update_value_text_content(ctx);
		game::menu::align_text(ctx);
		game::menu::update_text_tweens(ctx);
		
		// Save v-sync config
		(*ctx->config)["v_sync"] = v_sync;
	};
	
	auto increase_font_size_callback = [ctx]()
	{
		// Increase font size
		if (ctx->controls["menu_modifier"]->is_active())
			ctx->font_size += 0.01f;
		else
			ctx->font_size += 0.1f;
		
		// Limit font size
		if (ctx->font_size > 2.0f)
			ctx->font_size = 2.0f;
		
		// Update value text
		update_value_text_content(ctx);
		
		// Update config
		(*ctx->config)["font_size"] = ctx->font_size;
		
		// Reload fonts
		ctx->logger->push_task("Reloading fonts");
		try
		{
			game::load_fonts(ctx);
		}
		catch (...)
		{
			ctx->logger->pop_task(EXIT_FAILURE);
		}
		ctx->logger->pop_task(EXIT_SUCCESS);
		
		// Refresh and realign text
		game::menu::refresh_text(ctx);
		game::menu::align_text(ctx);
		game::menu::update_text_tweens(ctx);
	};
	
	auto decrease_font_size_callback = [ctx]()
	{
		// Decrease font size
		if (ctx->controls["menu_modifier"]->is_active())
			ctx->font_size -= 0.01f;
		else
			ctx->font_size -= 0.1f;
		
		// Limit font size
		if (ctx->font_size < 0.1f)
			ctx->font_size = 0.1f;
		
		// Update value text
		update_value_text_content(ctx);
		
		// Update config
		(*ctx->config)["font_size"] = ctx->font_size;
		
		// Reload fonts
		ctx->logger->push_task("Reloading fonts");
		try
		{
			game::load_fonts(ctx);
		}
		catch (...)
		{
			ctx->logger->pop_task(EXIT_FAILURE);
		}
		ctx->logger->pop_task(EXIT_SUCCESS);
		
		// Refresh and realign text
		game::menu::refresh_text(ctx);
		game::menu::align_text(ctx);
		game::menu::update_text_tweens(ctx);
	};
	
	auto toggle_dyslexia_font_callback = [ctx]()
	{
		ctx->dyslexia_font = !ctx->dyslexia_font;
		
		// Update value text
		update_value_text_content(ctx);
		
		// Save dyslexia font config
		(*ctx->config)["dyslexia_font"] = ctx->dyslexia_font;
		
		// Reload fonts
		ctx->logger->push_task("Reloading fonts");
		try
		{
			game::load_fonts(ctx);
		}
		catch (...)
		{
			ctx->logger->pop_task(EXIT_FAILURE);
		}
		ctx->logger->pop_task(EXIT_SUCCESS);
		
		// Refresh and realign text
		game::menu::refresh_text(ctx);
		game::menu::align_text(ctx);
		game::menu::update_text_tweens(ctx);
	};
	auto select_back_callback = [ctx]()
	{
		// Disable controls
		game::menu::clear_controls(ctx);
		
		game::menu::fade_out
		(
			ctx,
			[ctx]()
			{
				application::state next_state;
				next_state.name = "options_menu";
				next_state.enter = std::bind(game::state::options_menu::enter, ctx);
				next_state.exit = std::bind(game::state::options_menu::exit, ctx);
				ctx->app->queue_state(next_state);
			}
		);
	};
	
	// Build list of menu select callbacks
	ctx->menu_select_callbacks.push_back(toggle_fullscreen_callback);
	ctx->menu_select_callbacks.push_back(increase_resolution_callback);
	ctx->menu_select_callbacks.push_back(toggle_v_sync_callback);
	ctx->menu_select_callbacks.push_back(increase_font_size_callback);
	ctx->menu_select_callbacks.push_back(toggle_dyslexia_font_callback);
	ctx->menu_select_callbacks.push_back(select_back_callback);
	
	// Build list of menu left callbacks
	ctx->menu_left_callbacks.push_back(toggle_fullscreen_callback);
	ctx->menu_left_callbacks.push_back(decrease_resolution_callback);
	ctx->menu_left_callbacks.push_back(toggle_v_sync_callback);
	ctx->menu_left_callbacks.push_back(decrease_font_size_callback);
	ctx->menu_left_callbacks.push_back(toggle_dyslexia_font_callback);
	ctx->menu_left_callbacks.push_back(nullptr);
	
	// Build list of menu right callbacks
	ctx->menu_right_callbacks.push_back(toggle_fullscreen_callback);
	ctx->menu_right_callbacks.push_back(increase_resolution_callback);
	ctx->menu_right_callbacks.push_back(toggle_v_sync_callback);
	ctx->menu_right_callbacks.push_back(increase_font_size_callback);
	ctx->menu_right_callbacks.push_back(toggle_dyslexia_font_callback);
	ctx->menu_right_callbacks.push_back(nullptr);
	
	// Set menu back callback
	ctx->menu_back_callback = select_back_callback;
	
	// Schedule menu control setup
	timeline* timeline = ctx->timeline;
	float t = timeline->get_position();
	timeline->add_sequence({{t + game::menu::input_delay, std::bind(game::menu::setup_controls, ctx)}});
	
	// Fade in menu
	game::menu::fade_in(ctx, nullptr);
}

void exit(game::context* ctx)
{
	// Destruct menu
	game::menu::clear_controls(ctx);
	game::menu::clear_callbacks(ctx);
	game::menu::delete_animations(ctx);
	game::menu::remove_text_from_ui(ctx);
	game::menu::delete_text(ctx);
}

} // namespace graphics_menu
} // namespace state
} // namespace game

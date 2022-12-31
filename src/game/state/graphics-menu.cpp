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
#include "application.hpp"
#include "scene/text.hpp"
#include "debug/logger.hpp"
#include "game/fonts.hpp"
#include "game/menu.hpp"
#include "game/graphics.hpp"
#include "animation/timeline.hpp"

namespace game {
namespace state {

static void update_value_text_content(game::context* ctx);

graphics_menu::graphics_menu(game::context& ctx):
	game::state::base(ctx)
{
	ctx.logger->push_task("Entering graphics menu state");
	
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
	ctx.menu_item_texts.push_back({fullscreen_name_text, fullscreen_value_text});
	ctx.menu_item_texts.push_back({resolution_name_text, resolution_value_text});
	ctx.menu_item_texts.push_back({v_sync_name_text, v_sync_value_text});
	ctx.menu_item_texts.push_back({font_size_name_text, font_size_value_text});
	ctx.menu_item_texts.push_back({dyslexia_font_name_text, dyslexia_font_value_text});
	ctx.menu_item_texts.push_back({back_text, nullptr});
	
	// Set content of menu item texts
	fullscreen_name_text->set_content((*ctx.strings)["graphics_menu_fullscreen"]);
	resolution_name_text->set_content((*ctx.strings)["graphics_menu_resolution"]);
	v_sync_name_text->set_content((*ctx.strings)["graphics_menu_v_sync"]);
	font_size_name_text->set_content((*ctx.strings)["graphics_menu_font_size"]);
	dyslexia_font_name_text->set_content((*ctx.strings)["graphics_menu_dyslexia_font"]);
	back_text->set_content((*ctx.strings)["back"]);
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
		bool fullscreen = !ctx.app->is_fullscreen();
		
		ctx.app->set_fullscreen(fullscreen);
		
		if (!fullscreen)
		{
			int2 resolution;
			resolution.x() = (*ctx.config)["windowed_resolution"][0].get<int>();
			resolution.y() = (*ctx.config)["windowed_resolution"][1].get<int>();
			
			ctx.app->resize_window(resolution.x(), resolution.y());
		}
		
		this->update_value_text_content();
		game::menu::align_text(ctx);
		game::menu::update_text_tweens(ctx);
		
		// Save display mode config
		(*ctx.config)["fullscreen"] = fullscreen;
	};
	
	auto increase_resolution_callback = [this, &ctx]()
	{
		// Increase resolution
		if (ctx.controls["menu_modifier"]->is_active())
			ctx.render_resolution_scale += 0.05f;
		else
			ctx.render_resolution_scale += 0.25f;
		
		// Limit resolution
		if (ctx.render_resolution_scale > 2.0f)
			ctx.render_resolution_scale = 2.0f;
		
		// Resize framebuffers
		game::graphics::change_render_resolution(ctx, ctx.render_resolution_scale);
		
		// Update text
		this->update_value_text_content();
		game::menu::align_text(ctx);
		game::menu::update_text_tweens(ctx);
		
		// Update config
		(*ctx.config)["render_resolution"] = ctx.render_resolution_scale;
	};
	
	auto decrease_resolution_callback = [this, &ctx]()
	{
		// Increase resolution
		if (ctx.controls["menu_modifier"]->is_active())
			ctx.render_resolution_scale -= 0.05f;
		else
			ctx.render_resolution_scale -= 0.25f;
		
		// Limit resolution
		if (ctx.render_resolution_scale < 0.25f)
			ctx.render_resolution_scale = 0.25f;
		
		// Resize framebuffers
		game::graphics::change_render_resolution(ctx, ctx.render_resolution_scale);
		
		// Update text
		this->update_value_text_content();
		game::menu::align_text(ctx);
		game::menu::update_text_tweens(ctx);
		
		// Update config
		(*ctx.config)["render_resolution"] = ctx.render_resolution_scale;
	};
	
	auto toggle_v_sync_callback = [this, &ctx]()
	{
		bool v_sync = !ctx.app->get_v_sync();
		
		ctx.app->set_v_sync(v_sync);
		
		this->update_value_text_content();
		game::menu::align_text(ctx);
		game::menu::update_text_tweens(ctx);
		
		// Save v-sync config
		(*ctx.config)["v_sync"] = v_sync;
	};
	
	auto increase_font_size_callback = [this, &ctx]()
	{
		// Increase font size
		if (ctx.controls["menu_modifier"]->is_active())
			ctx.font_size += 0.01f;
		else
			ctx.font_size += 0.1f;
		
		// Limit font size
		if (ctx.font_size > 2.0f)
			ctx.font_size = 2.0f;
		
		// Update value text
		this->update_value_text_content();
		
		// Update config
		(*ctx.config)["font_size"] = ctx.font_size;
		
		// Reload fonts
		ctx.logger->push_task("Reloading fonts");
		try
		{
			game::load_fonts(ctx);
		}
		catch (...)
		{
			ctx.logger->pop_task(EXIT_FAILURE);
		}
		ctx.logger->pop_task(EXIT_SUCCESS);
		
		// Refresh and realign text
		game::menu::refresh_text(ctx);
		game::menu::align_text(ctx);
		game::menu::update_text_tweens(ctx);
	};
	
	auto decrease_font_size_callback = [this, &ctx]()
	{
		// Decrease font size
		if (ctx.controls["menu_modifier"]->is_active())
			ctx.font_size -= 0.01f;
		else
			ctx.font_size -= 0.1f;
		
		// Limit font size
		if (ctx.font_size < 0.1f)
			ctx.font_size = 0.1f;
		
		// Update value text
		this->update_value_text_content();
		
		// Update config
		(*ctx.config)["font_size"] = ctx.font_size;
		
		// Reload fonts
		ctx.logger->push_task("Reloading fonts");
		try
		{
			game::load_fonts(ctx);
		}
		catch (...)
		{
			ctx.logger->pop_task(EXIT_FAILURE);
		}
		ctx.logger->pop_task(EXIT_SUCCESS);
		
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
		
		// Save dyslexia font config
		(*ctx.config)["dyslexia_font"] = ctx.dyslexia_font;
		
		// Reload fonts
		ctx.logger->push_task("Reloading fonts");
		try
		{
			game::load_fonts(ctx);
		}
		catch (...)
		{
			ctx.logger->pop_task(EXIT_FAILURE);
		}
		ctx.logger->pop_task(EXIT_SUCCESS);
		
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
	ctx.menu_select_callbacks.push_back(increase_font_size_callback);
	ctx.menu_select_callbacks.push_back(toggle_dyslexia_font_callback);
	ctx.menu_select_callbacks.push_back(select_back_callback);
	
	// Build list of menu left callbacks
	ctx.menu_left_callbacks.push_back(toggle_fullscreen_callback);
	ctx.menu_left_callbacks.push_back(decrease_resolution_callback);
	ctx.menu_left_callbacks.push_back(toggle_v_sync_callback);
	ctx.menu_left_callbacks.push_back(decrease_font_size_callback);
	ctx.menu_left_callbacks.push_back(toggle_dyslexia_font_callback);
	ctx.menu_left_callbacks.push_back(nullptr);
	
	// Build list of menu right callbacks
	ctx.menu_right_callbacks.push_back(toggle_fullscreen_callback);
	ctx.menu_right_callbacks.push_back(increase_resolution_callback);
	ctx.menu_right_callbacks.push_back(toggle_v_sync_callback);
	ctx.menu_right_callbacks.push_back(increase_font_size_callback);
	ctx.menu_right_callbacks.push_back(toggle_dyslexia_font_callback);
	ctx.menu_right_callbacks.push_back(nullptr);
	
	// Set menu back callback
	ctx.menu_back_callback = select_back_callback;
	
	// Queue menu control setup
	ctx.function_queue.push(std::bind(game::menu::setup_controls, std::ref(ctx)));
	
	// Fade in menu
	game::menu::fade_in(ctx, nullptr);
	
	ctx.logger->pop_task(EXIT_SUCCESS);
}

graphics_menu::~graphics_menu()
{
	ctx.logger->push_task("Exiting graphics menu state");
	
	// Destruct menu
	game::menu::clear_controls(ctx);
	game::menu::clear_callbacks(ctx);
	game::menu::delete_animations(ctx);
	game::menu::remove_text_from_ui(ctx);
	game::menu::delete_text(ctx);
	
	ctx.logger->pop_task(EXIT_SUCCESS);
}

void graphics_menu::update_value_text_content()
{
	bool fullscreen = ctx.app->is_fullscreen();
	float resolution = ctx.render_resolution_scale;
	bool v_sync = ctx.app->get_v_sync();
	float font_size = ctx.font_size;
	bool dyslexia_font = ctx.dyslexia_font;
	
	const std::string string_on = (*ctx.strings)["on"];
	const std::string string_off = (*ctx.strings)["off"];
	
	std::get<1>(ctx.menu_item_texts[0])->set_content((fullscreen) ? string_on : string_off);
	std::get<1>(ctx.menu_item_texts[1])->set_content(std::to_string(static_cast<int>(std::round(resolution * 100.0f))) + "%");
	std::get<1>(ctx.menu_item_texts[2])->set_content((v_sync) ? string_on : string_off);
	std::get<1>(ctx.menu_item_texts[3])->set_content(std::to_string(static_cast<int>(std::round(font_size * 100.0f))) + "%");
	std::get<1>(ctx.menu_item_texts[4])->set_content((dyslexia_font) ? string_on : string_off);
}

} // namespace state
} // namespace game

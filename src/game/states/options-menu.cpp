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

#include "game/states/options-menu.hpp"
#include "game/states/main-menu.hpp"
#include "game/states/controls-menu.hpp"
#include "game/states/graphics-menu.hpp"
#include "game/states/sound-menu.hpp"
#include "game/states/language-menu.hpp"
#include "game/states/pause-menu.hpp"
#include "game/save.hpp"
#include "game/menu.hpp"
#include "animation/ease.hpp"
#include "animation/animation.hpp"
#include "animation/animator.hpp"
#include "animation/timeline.hpp"
#include "application.hpp"
#include "scene/text.hpp"

namespace game {
namespace state {
namespace options_menu {

void enter(game::context* ctx)
{
	// Construct menu item texts
	scene::text* controls_text = new scene::text();
	scene::text* graphics_text = new scene::text();
	scene::text* sound_text = new scene::text();
	scene::text* language_text = new scene::text();
	scene::text* back_text = new scene::text();
	
	// Set content of menu item texts
	controls_text->set_content((*ctx->strings)["options_menu_controls"]);
	graphics_text->set_content((*ctx->strings)["options_menu_graphics"]);
	sound_text->set_content((*ctx->strings)["options_menu_sound"]);
	language_text->set_content((*ctx->strings)["options_menu_language"]);
	back_text->set_content((*ctx->strings)["back"]);
	
	// Build list of menu item texts
	ctx->menu_item_texts.push_back({controls_text, nullptr});
	ctx->menu_item_texts.push_back({graphics_text, nullptr});
	ctx->menu_item_texts.push_back({sound_text, nullptr});
	ctx->menu_item_texts.push_back({language_text, nullptr});
	ctx->menu_item_texts.push_back({back_text, nullptr});
	
	// Init menu item index
	game::menu::init_menu_item_index(ctx, "options");
	
	game::menu::update_text_color(ctx);
	game::menu::update_text_font(ctx);
	game::menu::align_text(ctx, true);
	game::menu::update_text_tweens(ctx);
	game::menu::add_text_to_ui(ctx);
	game::menu::setup_animations(ctx);
	
	// Construct menu item callbacks
	auto select_controls_callback = [ctx]()
	{
		// Disable controls
		game::menu::clear_controls(ctx);
		
		// Return to main menu
		game::menu::fade_out
		(
			ctx,
			[ctx]()
			{
				application::state next_state;
				next_state.name = "controls_menu";
				next_state.enter = std::bind(game::state::controls_menu::enter, ctx);
				next_state.exit = std::bind(game::state::controls_menu::exit, ctx);
				ctx->app->queue_state(next_state);
			}
		);
	};
	auto select_graphics_callback = [ctx]()
	{
		// Disable controls
		game::menu::clear_controls(ctx);
		
		// Return to main menu
		game::menu::fade_out
		(
			ctx,
			[ctx]()
			{
				application::state next_state;
				next_state.name = "graphics_menu";
				next_state.enter = std::bind(game::state::graphics_menu::enter, ctx);
				next_state.exit = std::bind(game::state::graphics_menu::exit, ctx);
				ctx->app->queue_state(next_state);
			}
		);
	};
	auto select_sound_callback = [ctx]()
	{
		// Disable controls
		game::menu::clear_controls(ctx);
		
		// Return to main menu
		game::menu::fade_out
		(
			ctx,
			[ctx]()
			{
				application::state next_state;
				next_state.name = "sound_menu";
				next_state.enter = std::bind(game::state::sound_menu::enter, ctx);
				next_state.exit = std::bind(game::state::sound_menu::exit, ctx);
				ctx->app->queue_state(next_state);
			}
		);
	};
	auto select_language_callback = [ctx]()
	{
		// Disable controls
		game::menu::clear_controls(ctx);
		
		// Return to main menu
		game::menu::fade_out
		(
			ctx,
			[ctx]()
			{
				application::state next_state;
				next_state.name = "language_menu";
				next_state.enter = std::bind(game::state::language_menu::enter, ctx);
				next_state.exit = std::bind(game::state::language_menu::exit, ctx);
				ctx->app->queue_state(next_state);
			}
		);
	};
	auto select_back_callback = [ctx]()
	{
		// Disable controls
		game::menu::clear_controls(ctx);
		
		// Save config
		game::save_config(ctx);
		
		application::state next_state;
		if (ctx->paused_state)
		{
			// Return to pause menu
			next_state.name = "pause_menu";
			next_state.enter = std::bind(game::state::pause_menu::enter, ctx);
			next_state.exit = std::bind(game::state::pause_menu::exit, ctx);
		}
		else
		{
			// Return to main menu
			next_state.name = "main_menu";
			next_state.enter = std::bind(game::state::main_menu::enter, ctx, false);
			next_state.exit = std::bind(game::state::main_menu::exit, ctx);
		}
		
		game::menu::fade_out(ctx, std::bind(&application::queue_state, ctx->app, next_state));
	};
	
	// Build list of menu select callbacks
	ctx->menu_select_callbacks.push_back(select_controls_callback);
	ctx->menu_select_callbacks.push_back(select_graphics_callback);
	ctx->menu_select_callbacks.push_back(select_sound_callback);
	ctx->menu_select_callbacks.push_back(select_language_callback);
	ctx->menu_select_callbacks.push_back(select_back_callback);
	
	// Build list of menu right callbacks
	ctx->menu_right_callbacks.resize(5, nullptr);
	
	// Build list of menu left callbacks
	ctx->menu_left_callbacks.resize(5, nullptr);
	
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
	
	// Save config
	game::save_config(ctx);
}

} // namespace options_menu
} // namespace state
} // namespace game

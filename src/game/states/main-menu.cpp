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

#include "game/states/main-menu.hpp"
#include "game/states/title.hpp"
#include "game/states/options-menu.hpp"
#include "game/states/forage.hpp"
#include "game/states/nuptial-flight.hpp"
#include "game/states/credits.hpp"
#include "game/menu.hpp"
#include "render/passes/clear-pass.hpp"
#include "resources/resource-manager.hpp"
#include "render/model.hpp"
#include "animation/animation.hpp"
#include "animation/animator.hpp"
#include "animation/screen-transition.hpp"
#include "animation/ease.hpp"
#include "animation/timeline.hpp"
#include "application.hpp"
#include <limits>

namespace game {
namespace state {
namespace main_menu {

void enter(game::context* ctx)
{
	ctx->ui_clear_pass->set_cleared_buffers(true, true, false);
	
	// Construct menu item texts
	scene::text* start_text = new scene::text();
	scene::text* options_text = new scene::text();
	scene::text* credits_text = new scene::text();
	scene::text* quit_text = new scene::text();
	
	// Build list of menu item texts
	ctx->menu_item_texts.push_back({start_text, nullptr});
	ctx->menu_item_texts.push_back({options_text, nullptr});
	ctx->menu_item_texts.push_back({credits_text, nullptr});
	ctx->menu_item_texts.push_back({quit_text, nullptr});
	
	// Set content of menu item texts
	start_text->set_content((*ctx->strings)["main_menu_start"]);
	options_text->set_content((*ctx->strings)["main_menu_options"]);
	credits_text->set_content((*ctx->strings)["main_menu_credits"]);
	quit_text->set_content((*ctx->strings)["main_menu_quit"]);
	
	// Init menu item index
	game::menu::init_menu_item_index(ctx, "main");
	
	game::menu::update_text_color(ctx);
	game::menu::update_text_font(ctx);
	game::menu::align_text(ctx);
	game::menu::update_text_tweens(ctx);
	game::menu::add_text_to_ui(ctx);
	
	auto select_start_callback = [ctx]()
	{
		// Disable controls and menu callbacks
		game::menu::clear_controls(ctx);
		game::menu::clear_callbacks(ctx);
		
		// Create change state function
		auto change_state_nuptial_flight = [ctx]()
		{
			application::state next_state;
			next_state.name = "nuptial_flight";
			next_state.enter = std::bind(game::state::nuptial_flight::enter, ctx);
			next_state.exit = std::bind(game::state::nuptial_flight::exit, ctx);
			ctx->app->change_state(next_state);
		};
		
		// Set up timing
		const float fade_out_duration = 1.0f;
		
		// Schedule state change
		timeline* timeline = ctx->timeline;
		float t = timeline->get_position();
		timeline->add_sequence({{t + fade_out_duration, change_state_nuptial_flight}});
		
		// Start fade out to white
		ctx->fade_transition_color->set_value({1, 1, 1});
		ctx->fade_transition->transition(fade_out_duration, false, ease<float>::out_quad, false);
	};
	auto select_options_callback = [ctx]()
	{
		application::state next_state;
		next_state.name = "options_menu";
		next_state.enter = std::bind(game::state::options_menu::enter, ctx);
		next_state.exit = std::bind(game::state::options_menu::exit, ctx);
		ctx->app->change_state(next_state);
	};
	auto select_credits_callback = [ctx]()
	{
		application::state next_state;
		next_state.name = "credits";
		next_state.enter = std::bind(game::state::credits::enter, ctx);
		next_state.exit = std::bind(game::state::credits::exit, ctx);
		ctx->app->change_state(next_state);
	};
	auto select_quit_callback = [ctx]()
	{
		ctx->app->close(EXIT_SUCCESS);
	};
	auto menu_back_callback = [ctx]()
	{
		application::state next_state;
		next_state.name = "title";
		next_state.enter = std::bind(game::state::title::enter, ctx);
		next_state.exit = std::bind(game::state::title::exit, ctx);
		ctx->app->change_state(next_state);
	};
	
	// Build list of menu select callbacks
	ctx->menu_select_callbacks.push_back(select_start_callback);
	ctx->menu_select_callbacks.push_back(select_options_callback);
	ctx->menu_select_callbacks.push_back(select_credits_callback);
	ctx->menu_select_callbacks.push_back(select_quit_callback);
	
	// Build list of menu left callbacks
	ctx->menu_left_callbacks.push_back(nullptr);
	ctx->menu_left_callbacks.push_back(nullptr);
	ctx->menu_left_callbacks.push_back(nullptr);
	ctx->menu_left_callbacks.push_back(nullptr);
	
	// Build list of menu right callbacks
	ctx->menu_right_callbacks.push_back(nullptr);
	ctx->menu_right_callbacks.push_back(nullptr);
	ctx->menu_right_callbacks.push_back(nullptr);
	ctx->menu_right_callbacks.push_back(nullptr);
	
	// Set menu back callback
	ctx->menu_back_callback = menu_back_callback;
	
	// Schedule menu control setup
	timeline* timeline = ctx->timeline;
	float t = timeline->get_position();
	timeline->add_sequence({{t + game::menu::input_delay, std::bind(game::menu::setup_controls, ctx)}});
}

void exit(game::context* ctx)
{
	// Destruct menu
	game::menu::clear_controls(ctx);
	game::menu::clear_callbacks(ctx);
	game::menu::remove_text_from_ui(ctx);
	game::menu::delete_text(ctx);
	
	ctx->ui_clear_pass->set_cleared_buffers(false, true, false);
}

} // namespace main_menu
} // namespace state
} // namespace game

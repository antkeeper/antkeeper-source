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

#include "game/states/controls-menu.hpp"
#include "game/states/keyboard-config-menu.hpp"
#include "game/states/controller-config-menu.hpp"
#include "game/states/options-menu.hpp"
#include "application.hpp"
#include "scene/text.hpp"
#include "render/passes/clear-pass.hpp"
#include "debug/logger.hpp"
#include "game/menu.hpp"
#include "animation/timeline.hpp"

namespace game {
namespace state {
namespace controls_menu {

void enter(game::context* ctx)
{
	ctx->ui_clear_pass->set_cleared_buffers(true, true, false);
	
	// Construct menu item texts
	scene::text* keyboard_text = new scene::text();
	scene::text* controller_text = new scene::text();
	scene::text* back_text = new scene::text();
	
	// Build list of menu item texts
	ctx->menu_item_texts.push_back({keyboard_text, nullptr});
	ctx->menu_item_texts.push_back({controller_text, nullptr});
	ctx->menu_item_texts.push_back({back_text, nullptr});
	
	// Set content of menu item texts
	keyboard_text->set_content((*ctx->strings)["controls_menu_keyboard"]);
	controller_text->set_content((*ctx->strings)["controls_menu_controller"]);
	back_text->set_content((*ctx->strings)["back"]);
	
	// Init menu item index
	game::menu::init_menu_item_index(ctx, "controls");
	
	game::menu::update_text_color(ctx);
	game::menu::update_text_font(ctx);
	game::menu::align_text(ctx);
	game::menu::update_text_tweens(ctx);
	game::menu::add_text_to_ui(ctx);
	
	// Construct menu item callbacks
	auto select_keyboard_callback = [ctx]()
	{
		application::state next_state;
		next_state.name = "keyboard_config_menu";
		next_state.enter = std::bind(game::state::keyboard_config_menu::enter, ctx);
		next_state.exit = std::bind(game::state::keyboard_config_menu::exit, ctx);
		ctx->app->change_state(next_state);
	};
	auto select_controller_callback = [ctx]()
	{
		application::state next_state;
		next_state.name = "controller_config_menu";
		next_state.enter = std::bind(game::state::controller_config_menu::enter, ctx);
		next_state.exit = std::bind(game::state::controller_config_menu::exit, ctx);
		ctx->app->change_state(next_state);
	};
	auto select_back_callback = [ctx]()
	{
		application::state next_state;
		next_state.name = "options_menu";
		next_state.enter = std::bind(game::state::options_menu::enter, ctx);
		next_state.exit = std::bind(game::state::options_menu::exit, ctx);
		ctx->app->change_state(next_state);
	};
	
	// Build list of menu select callbacks
	ctx->menu_select_callbacks.push_back(select_keyboard_callback);
	ctx->menu_select_callbacks.push_back(select_controller_callback);
	ctx->menu_select_callbacks.push_back(select_back_callback);
	
	// Build list of menu left callbacks
	ctx->menu_left_callbacks.push_back(nullptr);
	ctx->menu_left_callbacks.push_back(nullptr);
	ctx->menu_left_callbacks.push_back(nullptr);
	
	// Build list of menu right callbacks
	ctx->menu_right_callbacks.push_back(nullptr);
	ctx->menu_right_callbacks.push_back(nullptr);
	ctx->menu_right_callbacks.push_back(nullptr);
	
	// Set menu back callback
	ctx->menu_back_callback = select_back_callback;
	
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

} // namespace controls_menu
} // namespace state
} // namespace game

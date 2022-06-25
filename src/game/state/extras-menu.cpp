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

#include "game/state/extras-menu.hpp"
#include "game/state/main-menu.hpp"
#include "game/state/credits.hpp"
#include "application.hpp"
#include "scene/text.hpp"
#include "debug/logger.hpp"
#include "game/fonts.hpp"
#include "game/menu.hpp"

namespace game {
namespace state {

extras_menu::extras_menu(game::context& ctx):
	game::state::base(ctx)
{
	ctx.logger->push_task("Entering extras menu state");
	
	// Construct menu item texts
	scene::text* credits_text = new scene::text();
	scene::text* back_text = new scene::text();
	
	// Build list of menu item texts
	ctx.menu_item_texts.push_back({credits_text, nullptr});
	ctx.menu_item_texts.push_back({back_text, nullptr});
	
	// Set content of menu item texts
	credits_text->set_content((*ctx.strings)["extras_menu_credits"]);
	back_text->set_content((*ctx.strings)["back"]);
	
	// Init menu item index
	game::menu::init_menu_item_index(ctx, "extras");
	
	game::menu::update_text_color(ctx);
	game::menu::update_text_font(ctx);
	game::menu::align_text(ctx);
	game::menu::update_text_tweens(ctx);
	game::menu::add_text_to_ui(ctx);
	game::menu::setup_animations(ctx);
	
	// Construct menu item callbacks
	auto select_credits_callback = [&ctx]()
	{
		// Disable controls
		game::menu::clear_controls(ctx);
		
		game::menu::fade_out
		(
			ctx,
			[&ctx]()
			{
				// Queue change to credits state
				ctx.function_queue.push
				(
					[&ctx]()
					{
						ctx.state_machine.pop();
						ctx.state_machine.emplace(new game::state::credits(ctx));
					}
				);
			}
		);
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
				// Queue change to main menu state
				ctx.function_queue.push
				(
					[&ctx]()
					{
						ctx.state_machine.pop();
						ctx.state_machine.emplace(new game::state::main_menu(ctx, false));
					}
				);
			}
		);
	};
	
	// Build list of menu select callbacks
	ctx.menu_select_callbacks.push_back(select_credits_callback);
	ctx.menu_select_callbacks.push_back(select_back_callback);
	
	// Build list of menu left callbacks
	ctx.menu_left_callbacks.push_back(nullptr);
	ctx.menu_left_callbacks.push_back(nullptr);
	
	// Build list of menu right callbacks
	ctx.menu_right_callbacks.push_back(nullptr);
	ctx.menu_right_callbacks.push_back(nullptr);
	
	// Set menu back callback
	ctx.menu_back_callback = select_back_callback;
	
	// Queue menu control setup
	ctx.function_queue.push(std::bind(game::menu::setup_controls, std::ref(ctx)));
	
	// Fade in menu
	game::menu::fade_in(ctx, nullptr);
	
	ctx.logger->pop_task(EXIT_SUCCESS);
}

extras_menu::~extras_menu()
{
	ctx.logger->push_task("Exiting extras menu state");
	
	// Destruct menu
	game::menu::clear_controls(ctx);
	game::menu::clear_callbacks(ctx);
	game::menu::delete_animations(ctx);
	game::menu::remove_text_from_ui(ctx);
	game::menu::delete_text(ctx);
	
	ctx.logger->pop_task(EXIT_SUCCESS);
}

} // namespace state
} // namespace game

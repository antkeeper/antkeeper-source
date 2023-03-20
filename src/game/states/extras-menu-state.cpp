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

#include "game/states/extras-menu-state.hpp"
#include "game/states/main-menu-state.hpp"
#include "game/states/credits-state.hpp"
#include "game/controls.hpp"
#include <engine/scene/text.hpp>
#include <engine/debug/log.hpp>
#include "game/fonts.hpp"
#include "game/menu.hpp"
#include "game/strings.hpp"
#include <engine/utility/hash/fnv1a.hpp>

using namespace hash::literals;

extras_menu_state::extras_menu_state(::game& ctx):
	game_state(ctx)
{
	debug::log::trace("Entering extras menu state...");
	
	// Construct menu item texts
	credits_text = std::make_unique<scene::text>();
	back_text = std::make_unique<scene::text>();
	
	// Build list of menu item texts
	ctx.menu_item_texts.push_back({credits_text.get(), nullptr});
	ctx.menu_item_texts.push_back({back_text.get(), nullptr});
	
	// Set content of menu item texts
	credits_text->set_content(get_string(ctx, "extras_menu_credits"));
	back_text->set_content(get_string(ctx, "back"));
	
	// Init menu item index
	::menu::init_menu_item_index(ctx, "extras");
	
	::menu::update_text_color(ctx);
	::menu::update_text_font(ctx);
	::menu::align_text(ctx);
	::menu::add_text_to_ui(ctx);
	::menu::setup_animations(ctx);
	
	// Construct menu item callbacks
	auto select_credits_callback = [&ctx]()
	{
		// Disable menu controls
		ctx.function_queue.push(std::bind(::disable_menu_controls, std::ref(ctx)));
		
		::menu::fade_out
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
						ctx.state_machine.emplace(std::make_unique<credits_state>(ctx));
					}
				);
			}
		);
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
				// Queue change to main menu state
				ctx.function_queue.push
				(
					[&ctx]()
					{
						ctx.state_machine.pop();
						ctx.state_machine.emplace(std::make_unique<main_menu_state>(ctx, false));
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
	
	// Fade in menu
	::menu::fade_in(ctx, nullptr);
	
	// Queue enable menu controls
	ctx.function_queue.push(std::bind(::enable_menu_controls, std::ref(ctx)));
	
	debug::log::trace("Entered extras menu state");
}

extras_menu_state::~extras_menu_state()
{
	debug::log::trace("Exiting extras menu state...");
	
	// Destruct menu
	::disable_menu_controls(ctx);
	::menu::clear_callbacks(ctx);
	::menu::delete_animations(ctx);
	::menu::remove_text_from_ui(ctx);
	::menu::delete_text(ctx);
	
	debug::log::trace("Exited extras menu state");
}


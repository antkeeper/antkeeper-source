// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include "game/states/extras-menu-state.hpp"
#include "game/states/main-menu-state.hpp"
#include "game/states/credits-state.hpp"
#include "game/controls.hpp"
#include <engine/scene/text.hpp>
#include <engine/debug/log.hpp>
#include "game/fonts.hpp"
#include "game/menu.hpp"
#include "game/strings.hpp"
#include <engine/hash/fnv.hpp>

using namespace hash::literals;

extras_menu_state::extras_menu_state(::game& ctx):
	game_state(ctx)
{
	debug::log_trace("Entering extras menu state...");
	
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
	::menu::init_menu_item_index(ctx, "extras"_fnv1a32);
	
	::menu::update_text_color(ctx);
	::menu::update_text_font(ctx);
	::menu::align_text(ctx);
	::menu::add_text_to_ui(ctx);
	
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
	
	debug::log_trace("Entered extras menu state");
}

extras_menu_state::~extras_menu_state()
{
	debug::log_trace("Exiting extras menu state...");
	
	// Destruct menu
	::disable_menu_controls(ctx);
	::menu::clear_callbacks(ctx);
	::menu::remove_text_from_ui(ctx);
	::menu::delete_text(ctx);
	
	debug::log_trace("Exited extras menu state");
}


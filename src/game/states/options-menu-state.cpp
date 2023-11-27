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

#include "game/states/options-menu-state.hpp"
#include "game/states/main-menu-state.hpp"
#include "game/states/controls-menu-state.hpp"
#include "game/states/graphics-menu-state.hpp"
#include "game/states/sound-menu-state.hpp"
#include "game/states/language-menu-state.hpp"
#include "game/states/pause-menu-state.hpp"
#include "game/menu.hpp"
#include "game/controls.hpp"
#include <engine/animation/ease.hpp>
#include <engine/animation/animation.hpp>
#include <engine/animation/animator.hpp>
#include <engine/scene/text.hpp>
#include <engine/debug/log.hpp>
#include "game/strings.hpp"
#include <engine/utility/hash/fnv1a.hpp>

using namespace hash::literals;

options_menu_state::options_menu_state(::game& ctx):
	game_state(ctx)
{
	debug::log_trace("Entering options menu state...");
	
	// Construct menu item texts
	controls_text = std::make_unique<scene::text>();
	graphics_text = std::make_unique<scene::text>();
	sound_text = std::make_unique<scene::text>();
	language_text = std::make_unique<scene::text>();
	back_text = std::make_unique<scene::text>();
	
	// Set content of menu item texts
	controls_text->set_content(get_string(ctx, "options_menu_controls"));
	graphics_text->set_content(get_string(ctx, "options_menu_graphics"));
	sound_text->set_content(get_string(ctx, "options_menu_sound"));
	language_text->set_content(get_string(ctx, "options_menu_language"));
	back_text->set_content(get_string(ctx, "back"));
	
	// Build list of menu item texts
	ctx.menu_item_texts.push_back({controls_text.get(), nullptr});
	ctx.menu_item_texts.push_back({graphics_text.get(), nullptr});
	ctx.menu_item_texts.push_back({sound_text.get(), nullptr});
	ctx.menu_item_texts.push_back({language_text.get(), nullptr});
	ctx.menu_item_texts.push_back({back_text.get(), nullptr});
	
	// Init menu item index
	::menu::init_menu_item_index(ctx, "options");
	
	::menu::update_text_color(ctx);
	::menu::update_text_font(ctx);
	::menu::align_text(ctx, true);
	::menu::add_text_to_ui(ctx);
	::menu::setup_animations(ctx);
	
	// Construct menu item callbacks
	auto select_controls_callback = [&ctx]()
	{
		// Disable menu controls
		ctx.function_queue.push(std::bind(::disable_menu_controls, std::ref(ctx)));
		
		// Return to main menu
		::menu::fade_out
		(
			ctx,
			[&ctx]()
			{
				// Queue change to controls menu state
				ctx.function_queue.push
				(
					[&ctx]()
					{
						ctx.state_machine.pop();
						ctx.state_machine.emplace(std::make_unique<controls_menu_state>(ctx));
					}
				);
			}
		);
	};
	auto select_graphics_callback = [&ctx]()
	{
		// Disable menu controls
		ctx.function_queue.push(std::bind(::disable_menu_controls, std::ref(ctx)));
		
		// Return to main menu
		::menu::fade_out
		(
			ctx,
			[&ctx]()
			{
				// Queue change to graphics menu state
				ctx.function_queue.push
				(
					[&ctx]()
					{
						ctx.state_machine.pop();
						ctx.state_machine.emplace(std::make_unique<graphics_menu_state>(ctx));
					}
				);
			}
		);
	};
	auto select_sound_callback = [&ctx]()
	{
		// Disable menu controls
		ctx.function_queue.push(std::bind(::disable_menu_controls, std::ref(ctx)));
		
		// Return to main menu
		::menu::fade_out
		(
			ctx,
			[&ctx]()
			{
				// Queue change to sound menu state
				ctx.function_queue.push
				(
					[&ctx]()
					{
						ctx.state_machine.pop();
						ctx.state_machine.emplace(std::make_unique<sound_menu_state>(ctx));
					}
				);
			}
		);
	};
	auto select_language_callback = [&ctx]()
	{
		// Disable menu controls
		ctx.function_queue.push(std::bind(::disable_menu_controls, std::ref(ctx)));
		
		// Return to main menu
		::menu::fade_out
		(
			ctx,
			[&ctx]()
			{
				// Queue change to language menu state
				ctx.function_queue.push
				(
					[&ctx]()
					{
						ctx.state_machine.pop();
						ctx.state_machine.emplace(std::make_unique<language_menu_state>(ctx));
					}
				);
			}
		);
	};
	auto select_back_callback = [&ctx]()
	{
		// Disable menu controls
		ctx.function_queue.push(std::bind(::disable_menu_controls, std::ref(ctx)));
		
		// Save config
		//::save::config(ctx);
		
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
						if (ctx.resume_callback)
							ctx.state_machine.emplace(std::make_unique<pause_menu_state>(ctx));
						else
							ctx.state_machine.emplace(std::make_unique<main_menu_state>(ctx, false));
					}
				);
			}
		);
	};
	
	// Build list of menu select callbacks
	ctx.menu_select_callbacks.push_back(select_controls_callback);
	ctx.menu_select_callbacks.push_back(select_graphics_callback);
	ctx.menu_select_callbacks.push_back(select_sound_callback);
	ctx.menu_select_callbacks.push_back(select_language_callback);
	ctx.menu_select_callbacks.push_back(select_back_callback);
	
	// Build list of menu right callbacks
	ctx.menu_right_callbacks.resize(5, nullptr);
	
	// Build list of menu left callbacks
	ctx.menu_left_callbacks.resize(5, nullptr);
	
	// Set menu back callback
	ctx.menu_back_callback = select_back_callback;
	
	// Fade in menu
	::menu::fade_in(ctx, nullptr);
	
	// Queue enable menu controls
	ctx.function_queue.push(std::bind(::enable_menu_controls, std::ref(ctx)));
	
	debug::log_trace("Entered options menu state");
}

options_menu_state::~options_menu_state()
{
	debug::log_trace("Exiting options menu state...");
	
	// Destruct menu
	::disable_menu_controls(ctx);
	::menu::clear_callbacks(ctx);
	::menu::delete_animations(ctx);
	::menu::remove_text_from_ui(ctx);
	::menu::delete_text(ctx);
	
	debug::log_trace("Exited options menu state");
}

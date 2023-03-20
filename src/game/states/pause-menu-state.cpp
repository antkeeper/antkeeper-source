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

#include "game/states/pause-menu-state.hpp"
#include "game/states/main-menu-state.hpp"
#include "game/states/options-menu-state.hpp"
#include "game/states/nuptial-flight-state.hpp"
#include "game/menu.hpp"
#include "game/controls.hpp"
#include <engine/animation/ease.hpp>
#include <engine/animation/animation.hpp>
#include <engine/animation/animator.hpp>
#include <engine/scene/text.hpp>
#include <engine/debug/log.hpp>
#include <engine/animation/screen-transition.hpp>
#include <engine/config.hpp>
#include "game/strings.hpp"
#include <engine/utility/hash/fnv1a.hpp>

using namespace hash::literals;

pause_menu_state::pause_menu_state(::game& ctx):
	game_state(ctx)
{
	debug::log::trace("Entering pause menu state...");
	
	// Construct menu item texts
	resume_text = std::make_unique<scene::text>();;
	options_text = std::make_unique<scene::text>();;
	main_menu_text = std::make_unique<scene::text>();;
	quit_text = std::make_unique<scene::text>();;
	
	// Set content of menu item texts
	resume_text->set_content(get_string(ctx, "pause_menu_resume"));
	options_text->set_content(get_string(ctx, "pause_menu_options"));
	main_menu_text->set_content(get_string(ctx, "pause_menu_main_menu"));
	quit_text->set_content(get_string(ctx, "pause_menu_quit"));
	
	// Build list of menu item texts
	ctx.menu_item_texts.push_back({resume_text.get(), nullptr});
	ctx.menu_item_texts.push_back({options_text.get(), nullptr});
	ctx.menu_item_texts.push_back({main_menu_text.get(), nullptr});
	ctx.menu_item_texts.push_back({quit_text.get(), nullptr});
	
	// Init menu item index
	::menu::init_menu_item_index(ctx, "pause");
	
	::menu::update_text_color(ctx);
	::menu::update_text_font(ctx);
	::menu::align_text(ctx, true, false);
	::menu::add_text_to_ui(ctx);
	::menu::setup_animations(ctx);
	
	// Construct menu item callbacks
	auto select_resume_callback = [&ctx]()
	{
		// Disable unpause control
		//ctx.controls["pause"]->set_activated_callback(nullptr);
		
		// Disable menu controls
		ctx.function_queue.push(std::bind(::disable_menu_controls, std::ref(ctx)));
		
		auto resume_paused_state = [&ctx]()
		{
			ctx.function_queue.push
			(
				[&ctx]()
				{
					ctx.state_machine.pop();
					ctx.resume_callback();
				}
			);
		};
		
		// Fade out pause menu then resume paused state
		::menu::fade_out(ctx, resume_paused_state);
		::menu::fade_out_bg(ctx);
	};
	auto select_options_callback = [&ctx]()
	{
		// Disable unpause control
		//ctx.controls["pause"]->set_activated_callback(nullptr);
		
		// Disable menu controls
		ctx.function_queue.push(std::bind(::disable_menu_controls, std::ref(ctx)));
		
		// Fade out pause menu then open options menu
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
	auto select_main_menu_callback = [&ctx]()
	{
		// Disable unpause control
		//ctx.controls["pause"]->set_activated_callback(nullptr);
		
		// Disable menu controls
		ctx.function_queue.push(std::bind(::disable_menu_controls, std::ref(ctx)));
		
		// Clear resume callback
		ctx.resume_callback = nullptr;
		
		auto fade_out_callback = [&ctx]()
		{
			// Queue change to main menu state
			ctx.function_queue.push
			(
				[&ctx]()
				{
					ctx.menu_bg_billboard->set_active(false);
					ctx.state_machine.pop();
					ctx.state_machine.pop();
					ctx.state_machine.emplace(std::make_unique<main_menu_state>(ctx, true));
				}
			);
		};
		
		// Fade out pause menu
		::menu::fade_out(ctx, nullptr);
		
		// Fade out to black then return to main menu
		ctx.fade_transition_color->set({0, 0, 0});
		ctx.fade_transition->transition(config::quit_fade_out_duration, false, ease<float>::out_cubic, false, fade_out_callback);
	};
	auto select_quit_callback = [&ctx]()
	{
		// Disable unpause control
		//ctx.controls["pause"]->set_activated_callback(nullptr);
		
		// Disable menu controls
		ctx.function_queue.push(std::bind(::disable_menu_controls, std::ref(ctx)));
		
		// Clear paused state
		//ctx.paused_state.reset();
		
		// Fade out pause menu
		::menu::fade_out(ctx, nullptr);
		
		// Fade out to black then quit
		ctx.fade_transition_color->set({0, 0, 0});
		ctx.fade_transition->transition(config::quit_fade_out_duration, false, ease<float>::out_cubic, false, [&ctx](){ctx.closed=true;});
	};
	
	// Build list of menu select callbacks
	ctx.menu_select_callbacks.push_back(select_resume_callback);
	ctx.menu_select_callbacks.push_back(select_options_callback);
	ctx.menu_select_callbacks.push_back(select_main_menu_callback);
	ctx.menu_select_callbacks.push_back(select_quit_callback);
	
	// Build list of menu right callbacks
	ctx.menu_right_callbacks.resize(4, nullptr);
	
	// Build list of menu left callbacks
	ctx.menu_left_callbacks.resize(4, nullptr);
	
	// Set menu back callback
	ctx.menu_back_callback = select_resume_callback;
	
	// Queue control setup
	ctx.function_queue.push
	(
		[&ctx, select_resume_callback]()
		{
			// Enable unpause control
			//ctx.controls["pause"]->set_activated_callback(select_resume_callback);
			
			// Enable menu controls
			::enable_menu_controls(ctx);
		}
	);
	
	// Fade in menu and menu BG
	::menu::fade_in(ctx, nullptr);
	if (!ctx.menu_bg_billboard->is_active())
		::menu::fade_in_bg(ctx);
	
	// Save colony
	//::save::colony(ctx);
	
	debug::log::trace("Entered pause menu state");
}

pause_menu_state::~pause_menu_state()
{
	debug::log::trace("Exiting pause menu state...");
	
	// Destruct menu
	::disable_menu_controls(ctx);
	::menu::clear_callbacks(ctx);
	::menu::delete_animations(ctx);
	::menu::remove_text_from_ui(ctx);
	::menu::delete_text(ctx);
	
	debug::log::trace("Exited pause menu state");
}

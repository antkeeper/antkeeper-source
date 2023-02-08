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

#include "game/state/options-menu.hpp"
#include "game/state/main-menu.hpp"
#include "game/state/controls-menu.hpp"
#include "game/state/graphics-menu.hpp"
#include "game/state/sound-menu.hpp"
#include "game/state/language-menu.hpp"
#include "game/state/pause-menu.hpp"
#include "game/menu.hpp"
#include "animation/ease.hpp"
#include "animation/animation.hpp"
#include "animation/animator.hpp"
#include "scene/text.hpp"
#include "debug/log.hpp"
#include "game/strings.hpp"
#include "utility/hash/fnv1a.hpp"

using namespace hash::literals;

namespace game {
namespace state {

options_menu::options_menu(game::context& ctx):
	game::state::base(ctx)
{
	debug::log::trace("Entering options menu state...");
	
	// Construct menu item texts
	scene::text* controls_text = new scene::text();
	scene::text* graphics_text = new scene::text();
	scene::text* sound_text = new scene::text();
	scene::text* language_text = new scene::text();
	scene::text* back_text = new scene::text();
	
	// Set content of menu item texts
	controls_text->set_content(get_string(ctx, "options_menu_controls"_fnv1a32));
	graphics_text->set_content(get_string(ctx, "options_menu_graphics"_fnv1a32));
	sound_text->set_content(get_string(ctx, "options_menu_sound"_fnv1a32));
	language_text->set_content(get_string(ctx, "options_menu_language"_fnv1a32));
	back_text->set_content(get_string(ctx, "back"_fnv1a32));
	
	// Build list of menu item texts
	ctx.menu_item_texts.push_back({controls_text, nullptr});
	ctx.menu_item_texts.push_back({graphics_text, nullptr});
	ctx.menu_item_texts.push_back({sound_text, nullptr});
	ctx.menu_item_texts.push_back({language_text, nullptr});
	ctx.menu_item_texts.push_back({back_text, nullptr});
	
	// Init menu item index
	game::menu::init_menu_item_index(ctx, "options");
	
	game::menu::update_text_color(ctx);
	game::menu::update_text_font(ctx);
	game::menu::align_text(ctx, true);
	game::menu::update_text_tweens(ctx);
	game::menu::add_text_to_ui(ctx);
	game::menu::setup_animations(ctx);
	
	// Construct menu item callbacks
	auto select_controls_callback = [&ctx]()
	{
		// Queue disable menu controls
		ctx.function_queue.push(std::bind(game::menu::disable_controls, std::ref(ctx)));
		
		// Return to main menu
		game::menu::fade_out
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
						ctx.state_machine.emplace(new game::state::controls_menu(ctx));
					}
				);
			}
		);
	};
	auto select_graphics_callback = [&ctx]()
	{
		// Queue disable menu controls
		ctx.function_queue.push(std::bind(game::menu::disable_controls, std::ref(ctx)));
		
		// Return to main menu
		game::menu::fade_out
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
						ctx.state_machine.emplace(new game::state::graphics_menu(ctx));
					}
				);
			}
		);
	};
	auto select_sound_callback = [&ctx]()
	{
		// Queue disable menu controls
		ctx.function_queue.push(std::bind(game::menu::disable_controls, std::ref(ctx)));
		
		// Return to main menu
		game::menu::fade_out
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
						ctx.state_machine.emplace(new game::state::sound_menu(ctx));
					}
				);
			}
		);
	};
	auto select_language_callback = [&ctx]()
	{
		// Queue disable menu controls
		ctx.function_queue.push(std::bind(game::menu::disable_controls, std::ref(ctx)));
		
		// Return to main menu
		game::menu::fade_out
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
						ctx.state_machine.emplace(new game::state::language_menu(ctx));
					}
				);
			}
		);
	};
	auto select_back_callback = [&ctx]()
	{
		// Queue disable menu controls
		ctx.function_queue.push(std::bind(game::menu::disable_controls, std::ref(ctx)));
		
		// Save config
		//game::save::config(ctx);
		
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
						if (ctx.resume_callback)
							ctx.state_machine.emplace(new game::state::pause_menu(ctx));
						else
							ctx.state_machine.emplace(new game::state::main_menu(ctx, false));
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
	game::menu::fade_in(ctx, nullptr);
	
	// Queue enable menu controls
	ctx.function_queue.push(std::bind(game::menu::enable_controls, std::ref(ctx)));
	
	debug::log::trace("Entered options menu state");
}

options_menu::~options_menu()
{
	debug::log::trace("Exiting options menu state...");
	
	// Destruct menu
	game::menu::clear_callbacks(ctx);
	game::menu::delete_animations(ctx);
	game::menu::remove_text_from_ui(ctx);
	game::menu::delete_text(ctx);
	
	debug::log::trace("Exited options menu state");
}

} // namespace state
} // namespace game

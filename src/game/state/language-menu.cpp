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

#include "game/state/language-menu.hpp"
#include "game/state/options-menu.hpp"
#include "game/controls.hpp"
#include "scene/text.hpp"
#include "debug/log.hpp"
#include "game/fonts.hpp"
#include "game/menu.hpp"
#include "game/strings.hpp"
#include "utility/hash/fnv1a.hpp"
#include "resources/resource-manager.hpp"
#include <algorithm>
#include <cctype>

using namespace hash::literals;

namespace game {
namespace state {

language_menu::language_menu(game::context& ctx):
	game::state::base(ctx)
{
	debug::log::trace("Entering language menu state...");
	
	/// @TODO Don't hardcode this
	language_tags =
	{
		"en",
		"zh-Hans",
		"zh-Hant"
	};
	
	// Determine index of current language
	language_index = 0;
	for (std::size_t i = 0; i < language_tags.size(); ++i)
	{
		if (ctx.language_tag == language_tags[i])
		{
			language_index = i;
			break;
		}
	}
	
	// Construct menu item texts
	scene::text* language_name_text = new scene::text();
	scene::text* language_value_text = new scene::text();
	scene::text* back_text = new scene::text();
	
	// Build list of menu item texts
	ctx.menu_item_texts.push_back({language_name_text, language_value_text});
	ctx.menu_item_texts.push_back({back_text, nullptr});
	
	// Set content of menu item texts
	update_text_content();
	
	// Init menu item index
	game::menu::init_menu_item_index(ctx, "language");
	
	game::menu::update_text_color(ctx);
	game::menu::update_text_font(ctx);
	game::menu::align_text(ctx);
	game::menu::update_text_tweens(ctx);
	game::menu::add_text_to_ui(ctx);
	game::menu::setup_animations(ctx);
	
	auto change_language = [this, &ctx]()
	{
		const std::string& language_tag = this->language_tags[this->language_index];
		
		// Slugify language tag
		std::string language_slug = language_tag;
		std::transform
		(
			language_slug.begin(),
			language_slug.end(),
			language_slug.begin(),
			[](unsigned char c)
			{
				return std::tolower(c);
			}
		);
		
		// Load language strings
		ctx.string_map = ctx.resource_manager->load<i18n::string_map>(language_slug + ".str");
		
		// Update language settings
		ctx.language_tag = language_tag;
		(*ctx.settings)["language_tag"_fnv1a32] = ctx.language_tag;
		
		// Log language change
		debug::log::info("Language tag: {}", ctx.language_tag);
		
		// Reload fonts
		debug::log::trace("Reloading fonts...");
		game::load_fonts(ctx);
		debug::log::trace("Reloaded fonts");
		
		// Update menus
		game::menu::update_text_font(ctx);
		this->update_text_content();
		game::menu::refresh_text(ctx);
		game::menu::align_text(ctx);
		game::menu::update_text_tweens(ctx);
	};
	
	// Construct menu item callbacks
	auto next_language_callback = [this, &ctx, change_language]()
	{
		this->language_index = (this->language_index + 1) % this->language_tags.size();
		change_language();
	};
	auto previous_language_callback = [this, &ctx, change_language]()
	{
		if (this->language_index > 0)
		{
			--this->language_index;
		}
		else
		{
			this->language_index = this->language_tags.size() - 1;
		}
		
		change_language();
	};
	auto select_back_callback = [&ctx]()
	{
		// Disable menu controls
		ctx.function_queue.push(std::bind(game::disable_menu_controls, std::ref(ctx)));
		
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
	ctx.menu_select_callbacks.push_back(next_language_callback);
	ctx.menu_select_callbacks.push_back(select_back_callback);
	
	// Build list of menu left callbacks
	ctx.menu_left_callbacks.push_back(previous_language_callback);
	ctx.menu_left_callbacks.push_back(nullptr);
	
	// Build list of menu right callbacks
	ctx.menu_right_callbacks.push_back(next_language_callback);
	ctx.menu_right_callbacks.push_back(nullptr);
	
	// Set menu back callback
	ctx.menu_back_callback = select_back_callback;
	
	// Enable menu controls next frame
	ctx.function_queue.push(std::bind(game::enable_menu_controls, std::ref(ctx)));
	
	// Fade in menu
	game::menu::fade_in(ctx, nullptr);
	
	debug::log::trace("Entered language menu state");
}

language_menu::~language_menu()
{
	debug::log::trace("Exiting language menu state...");
	
	// Destruct menu
	game::disable_menu_controls(ctx);
	game::menu::clear_callbacks(ctx);
	game::menu::delete_animations(ctx);
	game::menu::remove_text_from_ui(ctx);
	game::menu::delete_text(ctx);
	
	debug::log::trace("Exited language menu state");
}

void language_menu::update_text_content()
{
	auto [language_name, language_value] = ctx.menu_item_texts[0];
	auto [back_name, back_value] = ctx.menu_item_texts[1];
	
	language_name->set_content(get_string(ctx, "language_menu_language"_fnv1a32));
	language_value->set_content(get_string(ctx, "language_name_native"_fnv1a32));
	back_name->set_content(get_string(ctx, "back"_fnv1a32));
}

} // namespace state
} // namespace game

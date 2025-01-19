// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include "game/states/language-menu-state.hpp"
#include "game/states/options-menu-state.hpp"
#include "game/controls.hpp"
#include <engine/scene/text.hpp>
#include <engine/debug/log.hpp>
#include "game/fonts.hpp"
#include "game/menu.hpp"
#include "game/strings.hpp"
#include "game/debug/shell.hpp"
#include <engine/hash/fnv.hpp>
#include <engine/resources/resource-manager.hpp>
#include <algorithm>
#include <cctype>

using namespace hash::literals;

language_menu_state::language_menu_state(::game& ctx):
	game_state(ctx)
{
	debug::log_trace("Entering language menu state...");
	
	// Get iterator to current language
	if (language_it = ctx.languages->find(ctx.language_tag); language_it == ctx.languages->end())
	{
		throw std::runtime_error("Language not found");
	}
	
	// Construct menu item texts
	language_name_text = std::make_unique<scene::text>();
	language_value_text = std::make_unique<scene::text>();
	back_text = std::make_unique<scene::text>();
	
	// Build list of menu item texts
	ctx.menu_item_texts.push_back({language_name_text.get(), language_value_text.get()});
	ctx.menu_item_texts.push_back({back_text.get(), nullptr});
	
	// Set content of menu item texts
	update_text_content();
	
	// Init menu item index
	::menu::init_menu_item_index(ctx, "language"_fnv1a32);
	
	::menu::update_text_color(ctx);
	::menu::update_text_font(ctx);
	::menu::align_text(ctx);
	::menu::add_text_to_ui(ctx);
	
	auto change_language = [&]()
	{
		// Get language tag from language key
		ctx.language_tag = language_it.key();
		
		// Load language strings
		ctx.string_map = ctx.resource_manager->load<json>(std::format("localization/strings.{}.json", ctx.language_tag));
		
		// Update language tag settings
		(*ctx.settings)["language_tag"] = ctx.language_tag;

		// Log language tag
		debug::log_info("Language tag: {}", ctx.language_tag);
		
		// Reload fonts
		debug::log_trace("Reloading fonts...");
		::load_fonts(ctx);
		debug::log_trace("Reloaded fonts");
		
		// Update menus
		::menu::update_text_font(ctx);
		update_text_content();
		::menu::refresh_text(ctx);
		::menu::align_text(ctx);
	};
	
	// Construct menu item callbacks
	auto next_language_callback = [this, &ctx, change_language]()
	{
		if (++language_it; language_it == ctx.languages->end())
		{
			language_it = ctx.languages->begin();
		}
		
		change_language();
	};
	auto previous_language_callback = [this, &ctx, change_language]()
	{
		if (language_it != ctx.languages->begin())
		{
			--language_it;
		}
		else
		{
			language_it = ctx.languages->end();
			--language_it;
		}
		
		change_language();
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
	ctx.function_queue.push(std::bind(::enable_menu_controls, std::ref(ctx)));
	
	// Fade in menu
	::menu::fade_in(ctx, nullptr);
	
	debug::log_trace("Entered language menu state");
}

language_menu_state::~language_menu_state()
{
	debug::log_trace("Exiting language menu state...");
	
	// Destruct menu
	::disable_menu_controls(ctx);
	::menu::clear_callbacks(ctx);
	::menu::remove_text_from_ui(ctx);
	::menu::delete_text(ctx);
	
	debug::log_trace("Exited language menu state");
}

void language_menu_state::update_text_content()
{
	auto [language_name, language_value] = ctx.menu_item_texts[0];
	auto [back_name, back_value] = ctx.menu_item_texts[1];
	
	language_name->set_content(get_string(ctx, "language_menu_language"));
	language_value->set_content((*ctx.languages)[ctx.language_tag]["name"].get_ref<const std::string&>());
	back_name->set_content(get_string(ctx, "back"));
}

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

#include "game/states/sound-menu-state.hpp"
#include "game/states/options-menu-state.hpp"
#include "game/controls.hpp"
#include <engine/scene/text.hpp>
#include <engine/debug/log.hpp>
#include "game/menu.hpp"
#include "game/strings.hpp"
#include <engine/utility/hash/fnv1a.hpp>

using namespace hash::literals;


sound_menu_state::sound_menu_state(::game& ctx):
	game_state(ctx)
{
	debug::log::trace("Entering sound menu state...");
	
	// Construct menu item texts
	master_volume_name_text = std::make_unique<scene::text>();
	master_volume_value_text = std::make_unique<scene::text>();
	ambience_volume_name_text = std::make_unique<scene::text>();
	ambience_volume_value_text = std::make_unique<scene::text>();
	effects_volume_name_text = std::make_unique<scene::text>();
	effects_volume_value_text = std::make_unique<scene::text>();
	mono_audio_name_text = std::make_unique<scene::text>();
	mono_audio_value_text = std::make_unique<scene::text>();
	captions_name_text = std::make_unique<scene::text>();
	captions_value_text = std::make_unique<scene::text>();
	captions_size_name_text = std::make_unique<scene::text>();
	captions_size_value_text = std::make_unique<scene::text>();
	back_text = std::make_unique<scene::text>();
	
	// Build list of menu item texts
	ctx.menu_item_texts.push_back({master_volume_name_text.get(), master_volume_value_text.get()});
	ctx.menu_item_texts.push_back({ambience_volume_name_text.get(), ambience_volume_value_text.get()});
	ctx.menu_item_texts.push_back({effects_volume_name_text.get(), effects_volume_value_text.get()});
	ctx.menu_item_texts.push_back({mono_audio_name_text.get(), mono_audio_value_text.get()});
	ctx.menu_item_texts.push_back({captions_name_text.get(), captions_value_text.get()});
	ctx.menu_item_texts.push_back({captions_size_name_text.get(), captions_size_value_text.get()});
	ctx.menu_item_texts.push_back({back_text.get(), nullptr});
	
	// Set content of menu item texts
	master_volume_name_text->set_content(get_string(ctx, "sound_menu_master_volume"));
	ambience_volume_name_text->set_content(get_string(ctx, "sound_menu_ambience_volume"));
	effects_volume_name_text->set_content(get_string(ctx, "sound_menu_effects_volume"));
	mono_audio_name_text->set_content(get_string(ctx, "sound_menu_mono_audio"));
	captions_name_text->set_content(get_string(ctx, "sound_menu_captions"));
	captions_size_name_text->set_content(get_string(ctx, "sound_menu_captions_size"));
	back_text->set_content(get_string(ctx, "back"));
	update_value_text_content();
	
	// Init menu item index
	::menu::init_menu_item_index(ctx, "sound");
	
	::menu::update_text_color(ctx);
	::menu::update_text_font(ctx);
	::menu::align_text(ctx);
	::menu::update_text_tweens(ctx);
	::menu::add_text_to_ui(ctx);
	::menu::setup_animations(ctx);
	
	// Construct menu item callbacks
	auto increase_volume_callback = [this, &ctx](float* volume)
	{
		// Increase volume
		if (ctx.menu_modifier_action.is_active())
			*volume += 0.01f;
		else
			*volume += 0.1f;
		
		// Limit volume
		if (*volume > 1.0f)
			*volume = 1.0f;
		
		this->update_value_text_content();
		::menu::align_text(ctx);
		::menu::update_text_tweens(ctx);
	};
	auto decrease_volume_callback = [this, &ctx](float* volume)
	{
		// Decrease volume
		if (ctx.menu_modifier_action.is_active())
			*volume -= 0.01f;
		else
			*volume -= 0.1f;
		
		// Limit volume
		if (*volume < 0.0f)
			*volume = 0.0f;
		
		this->update_value_text_content();
		::menu::align_text(ctx);
		::menu::update_text_tweens(ctx);
	};
	
	auto toggle_mono_audio_callback = [this, &ctx]()
	{
		ctx.mono_audio = !ctx.mono_audio;
		
		this->update_value_text_content();
		::menu::align_text(ctx);
		::menu::update_text_tweens(ctx);
	};
	
	auto toggle_captions_callback = [this, &ctx]()
	{
		ctx.captions = !ctx.captions;
		
		this->update_value_text_content();
		::menu::align_text(ctx);
		::menu::update_text_tweens(ctx);
	};
	
	auto increase_captions_size_callback = [this, &ctx]()
	{
		// Increase size
		if (ctx.menu_modifier_action.is_active())
			ctx.captions_size += 0.01f;
		else
			ctx.captions_size += 0.1f;
		
		// Limit size
		if (ctx.captions_size > 2.0f)
			ctx.captions_size = 2.0f;
		
		this->update_value_text_content();
		::menu::align_text(ctx);
		::menu::update_text_tweens(ctx);
	};
	
	auto decrease_captions_size_callback = [this, &ctx]()
	{
		// Decrease size
		if (ctx.menu_modifier_action.is_active())
			ctx.captions_size -= 0.01f;
		else
			ctx.captions_size -= 0.1f;
		
		// Limit size
		if (ctx.captions_size < 0.1f)
			ctx.captions_size = 0.1f;
		
		this->update_value_text_content();
		::menu::align_text(ctx);
		::menu::update_text_tweens(ctx);
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
	ctx.menu_select_callbacks.push_back(std::bind(increase_volume_callback, &ctx.master_volume));
	ctx.menu_select_callbacks.push_back(std::bind(increase_volume_callback, &ctx.ambience_volume));
	ctx.menu_select_callbacks.push_back(std::bind(increase_volume_callback, &ctx.effects_volume));
	ctx.menu_select_callbacks.push_back(toggle_mono_audio_callback);
	ctx.menu_select_callbacks.push_back(toggle_captions_callback);
	ctx.menu_select_callbacks.push_back(increase_captions_size_callback);
	ctx.menu_select_callbacks.push_back(select_back_callback);
	
	// Build list of menu left callbacks
	ctx.menu_left_callbacks.push_back(std::bind(decrease_volume_callback, &ctx.master_volume));
	ctx.menu_left_callbacks.push_back(std::bind(decrease_volume_callback, &ctx.ambience_volume));
	ctx.menu_left_callbacks.push_back(std::bind(decrease_volume_callback, &ctx.effects_volume));
	ctx.menu_left_callbacks.push_back(toggle_mono_audio_callback);
	ctx.menu_left_callbacks.push_back(toggle_captions_callback);
	ctx.menu_left_callbacks.push_back(decrease_captions_size_callback);
	ctx.menu_left_callbacks.push_back(nullptr);
	
	// Build list of menu right callbacks
	ctx.menu_right_callbacks.push_back(std::bind(increase_volume_callback, &ctx.master_volume));
	ctx.menu_right_callbacks.push_back(std::bind(increase_volume_callback, &ctx.ambience_volume));
	ctx.menu_right_callbacks.push_back(std::bind(increase_volume_callback, &ctx.effects_volume));
	ctx.menu_right_callbacks.push_back(toggle_mono_audio_callback);
	ctx.menu_right_callbacks.push_back(toggle_captions_callback);
	ctx.menu_right_callbacks.push_back(increase_captions_size_callback);
	ctx.menu_right_callbacks.push_back(nullptr);
	
	// Set menu back callback
	ctx.menu_back_callback = select_back_callback;
	
	// Queue menu control setup
	ctx.function_queue.push(std::bind(::enable_menu_controls, std::ref(ctx)));
	
	// Fade in menu
	::menu::fade_in(ctx, nullptr);
	
	debug::log::trace("Entered sound menu state");
}

sound_menu_state::~sound_menu_state()
{
	debug::log::trace("Exiting sound menu state...");
	
	// Destruct menu
	::disable_menu_controls(ctx);
	::menu::clear_callbacks(ctx);
	::menu::delete_animations(ctx);
	::menu::remove_text_from_ui(ctx);
	::menu::delete_text(ctx);
	
	debug::log::trace("Exited sound menu state");
}

void sound_menu_state::update_value_text_content()
{
	const std::string string_on = get_string(ctx, "on");
	const std::string string_off = get_string(ctx, "off");
	
	std::get<1>(ctx.menu_item_texts[0])->set_content(std::to_string(static_cast<int>(std::round(ctx.master_volume * 100.0f))) + "%");
	std::get<1>(ctx.menu_item_texts[1])->set_content(std::to_string(static_cast<int>(std::round(ctx.ambience_volume * 100.0f))) + "%");
	std::get<1>(ctx.menu_item_texts[2])->set_content(std::to_string(static_cast<int>(std::round(ctx.effects_volume * 100.0f))) + "%");
	std::get<1>(ctx.menu_item_texts[3])->set_content((ctx.mono_audio) ? string_on : string_off);
	std::get<1>(ctx.menu_item_texts[4])->set_content((ctx.captions) ? string_on : string_off);
	std::get<1>(ctx.menu_item_texts[5])->set_content(std::to_string(static_cast<int>(std::round(ctx.captions_size * 100.0f))) + "%");
}

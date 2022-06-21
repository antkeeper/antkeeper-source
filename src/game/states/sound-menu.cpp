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

#include "game/states/sound-menu.hpp"
#include "game/states/options-menu.hpp"
#include "application.hpp"
#include "scene/text.hpp"
#include "debug/logger.hpp"
#include "game/menu.hpp"
#include "animation/timeline.hpp"

namespace game {
namespace state {
namespace sound_menu {

static void update_value_text_content(game::context* ctx)
{
	const std::string string_on = (*ctx->strings)["on"];
	const std::string string_off = (*ctx->strings)["off"];
	
	std::get<1>(ctx->menu_item_texts[0])->set_content(std::to_string(static_cast<int>(std::round(ctx->master_volume * 100.0f))) + "%");
	std::get<1>(ctx->menu_item_texts[1])->set_content(std::to_string(static_cast<int>(std::round(ctx->ambience_volume * 100.0f))) + "%");
	std::get<1>(ctx->menu_item_texts[2])->set_content(std::to_string(static_cast<int>(std::round(ctx->effects_volume * 100.0f))) + "%");
	std::get<1>(ctx->menu_item_texts[3])->set_content((ctx->mono_audio) ? string_on : string_off);
	std::get<1>(ctx->menu_item_texts[4])->set_content((ctx->captions) ? string_on : string_off);
	std::get<1>(ctx->menu_item_texts[5])->set_content(std::to_string(static_cast<int>(std::round(ctx->captions_size * 100.0f))) + "%");
}

void enter(game::context* ctx)
{
	// Construct menu item texts
	scene::text* master_volume_name_text = new scene::text();
	scene::text* master_volume_value_text = new scene::text();
	scene::text* ambience_volume_name_text = new scene::text();
	scene::text* ambience_volume_value_text = new scene::text();
	scene::text* effects_volume_name_text = new scene::text();
	scene::text* effects_volume_value_text = new scene::text();
	scene::text* mono_audio_name_text = new scene::text();
	scene::text* mono_audio_value_text = new scene::text();
	scene::text* captions_name_text = new scene::text();
	scene::text* captions_value_text = new scene::text();
	scene::text* captions_size_name_text = new scene::text();
	scene::text* captions_size_value_text = new scene::text();
	scene::text* back_text = new scene::text();
	
	// Build list of menu item texts
	ctx->menu_item_texts.push_back({master_volume_name_text, master_volume_value_text});
	ctx->menu_item_texts.push_back({ambience_volume_name_text, ambience_volume_value_text});
	ctx->menu_item_texts.push_back({effects_volume_name_text, effects_volume_value_text});
	ctx->menu_item_texts.push_back({mono_audio_name_text, mono_audio_value_text});
	ctx->menu_item_texts.push_back({captions_name_text, captions_value_text});
	ctx->menu_item_texts.push_back({captions_size_name_text, captions_size_value_text});
	ctx->menu_item_texts.push_back({back_text, nullptr});
	
	// Set content of menu item texts
	master_volume_name_text->set_content((*ctx->strings)["sound_menu_master_volume"]);
	ambience_volume_name_text->set_content((*ctx->strings)["sound_menu_ambience_volume"]);
	effects_volume_name_text->set_content((*ctx->strings)["sound_menu_effects_volume"]);
	mono_audio_name_text->set_content((*ctx->strings)["sound_menu_mono_audio"]);
	captions_name_text->set_content((*ctx->strings)["sound_menu_captions"]);
	captions_size_name_text->set_content((*ctx->strings)["sound_menu_captions_size"]);
	back_text->set_content((*ctx->strings)["back"]);
	update_value_text_content(ctx);
	
	// Init menu item index
	game::menu::init_menu_item_index(ctx, "sound");
	
	game::menu::update_text_color(ctx);
	game::menu::update_text_font(ctx);
	game::menu::align_text(ctx);
	game::menu::update_text_tweens(ctx);
	game::menu::add_text_to_ui(ctx);
	game::menu::setup_animations(ctx);
	
	// Construct menu item callbacks
	auto increase_volume_callback = [ctx](float* volume)
	{
		// Increase volume
		if (ctx->controls["menu_modifier"]->is_active())
			*volume += 0.01f;
		else
			*volume += 0.1f;
		
		// Limit volume
		if (*volume > 1.0f)
			*volume = 1.0f;
		
		update_value_text_content(ctx);
		game::menu::align_text(ctx);
		game::menu::update_text_tweens(ctx);
	};
	auto decrease_volume_callback = [ctx](float* volume)
	{
		// Decrease volume
		if (ctx->controls["menu_modifier"]->is_active())
			*volume -= 0.01f;
		else
			*volume -= 0.1f;
		
		// Limit volume
		if (*volume < 0.0f)
			*volume = 0.0f;
		
		update_value_text_content(ctx);
		game::menu::align_text(ctx);
		game::menu::update_text_tweens(ctx);
	};
	
	auto toggle_mono_audio_callback = [ctx]()
	{
		ctx->mono_audio = !ctx->mono_audio;
		
		update_value_text_content(ctx);
		game::menu::align_text(ctx);
		game::menu::update_text_tweens(ctx);
	};
	
	auto toggle_captions_callback = [ctx]()
	{
		ctx->captions = !ctx->captions;
		
		update_value_text_content(ctx);
		game::menu::align_text(ctx);
		game::menu::update_text_tweens(ctx);
	};
	
	auto increase_captions_size_callback = [ctx]()
	{
		// Increase size
		if (ctx->controls["menu_modifier"]->is_active())
			ctx->captions_size += 0.01f;
		else
			ctx->captions_size += 0.1f;
		
		// Limit size
		if (ctx->captions_size > 2.0f)
			ctx->captions_size = 2.0f;
		
		update_value_text_content(ctx);
		game::menu::align_text(ctx);
		game::menu::update_text_tweens(ctx);
	};
	
	auto decrease_captions_size_callback = [ctx]()
	{
		// Decrease size
		if (ctx->controls["menu_modifier"]->is_active())
			ctx->captions_size -= 0.01f;
		else
			ctx->captions_size -= 0.1f;
		
		// Limit size
		if (ctx->captions_size < 0.1f)
			ctx->captions_size = 0.1f;
		
		update_value_text_content(ctx);
		game::menu::align_text(ctx);
		game::menu::update_text_tweens(ctx);
	};
	auto select_back_callback = [ctx]()
	{
		// Disable controls
		game::menu::clear_controls(ctx);
		
		game::menu::fade_out
		(
			ctx,
			[ctx]()
			{
				application::state next_state;
				next_state.name = "options_menu";
				next_state.enter = std::bind(game::state::options_menu::enter, ctx);
				next_state.exit = std::bind(game::state::options_menu::exit, ctx);
				ctx->app->queue_state(next_state);
			}
		);
	};
	
	// Build list of menu select callbacks
	ctx->menu_select_callbacks.push_back(std::bind(increase_volume_callback, &ctx->master_volume));
	ctx->menu_select_callbacks.push_back(std::bind(increase_volume_callback, &ctx->ambience_volume));
	ctx->menu_select_callbacks.push_back(std::bind(increase_volume_callback, &ctx->effects_volume));
	ctx->menu_select_callbacks.push_back(toggle_mono_audio_callback);
	ctx->menu_select_callbacks.push_back(toggle_captions_callback);
	ctx->menu_select_callbacks.push_back(increase_captions_size_callback);
	ctx->menu_select_callbacks.push_back(select_back_callback);
	
	// Build list of menu left callbacks
	ctx->menu_left_callbacks.push_back(std::bind(decrease_volume_callback, &ctx->master_volume));
	ctx->menu_left_callbacks.push_back(std::bind(decrease_volume_callback, &ctx->ambience_volume));
	ctx->menu_left_callbacks.push_back(std::bind(decrease_volume_callback, &ctx->effects_volume));
	ctx->menu_left_callbacks.push_back(toggle_mono_audio_callback);
	ctx->menu_left_callbacks.push_back(toggle_captions_callback);
	ctx->menu_left_callbacks.push_back(decrease_captions_size_callback);
	ctx->menu_left_callbacks.push_back(nullptr);
	
	// Build list of menu right callbacks
	ctx->menu_right_callbacks.push_back(std::bind(increase_volume_callback, &ctx->master_volume));
	ctx->menu_right_callbacks.push_back(std::bind(increase_volume_callback, &ctx->ambience_volume));
	ctx->menu_right_callbacks.push_back(std::bind(increase_volume_callback, &ctx->effects_volume));
	ctx->menu_right_callbacks.push_back(toggle_mono_audio_callback);
	ctx->menu_right_callbacks.push_back(toggle_captions_callback);
	ctx->menu_right_callbacks.push_back(increase_captions_size_callback);
	ctx->menu_right_callbacks.push_back(nullptr);
	
	// Set menu back callback
	ctx->menu_back_callback = select_back_callback;
	
	// Schedule menu control setup
	timeline* timeline = ctx->timeline;
	float t = timeline->get_position();
	timeline->add_sequence({{t + game::menu::input_delay, std::bind(game::menu::setup_controls, ctx)}});
	
	// Fade in menu
	game::menu::fade_in(ctx, nullptr);
}

void exit(game::context* ctx)
{
	// Destruct menu
	game::menu::clear_controls(ctx);
	game::menu::clear_callbacks(ctx);
	game::menu::delete_animations(ctx);
	game::menu::remove_text_from_ui(ctx);
	game::menu::delete_text(ctx);
	
	// Update config
	(*ctx->config)["master_volume"] = ctx->master_volume;
	(*ctx->config)["ambience_volume"] = ctx->ambience_volume;
	(*ctx->config)["effects_volume"] = ctx->effects_volume;
	(*ctx->config)["mono_audio"] = ctx->mono_audio;
	(*ctx->config)["captions"] = ctx->captions;
	(*ctx->config)["captions_size"] = ctx->captions_size;
}

} // namespace sound_menu
} // namespace state
} // namespace game

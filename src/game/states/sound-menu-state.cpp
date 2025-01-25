// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include "game/states/sound-menu-state.hpp"
#include "game/states/options-menu-state.hpp"
#include "game/controls.hpp"
#include <engine/scene/text.hpp>
#include <engine/debug/log.hpp>
#include "game/menu.hpp"
#include "game/strings.hpp"
#include <engine/hash/fnv.hpp>

using namespace hash::literals;


sound_menu_state::sound_menu_state(::game& ctx):
	game_state(ctx)
{
	debug::log_trace("Entering sound menu state...");
	
	// Construct menu item texts
	master_volume_name_text = std::make_unique<scene::text>();
	master_volume_value_text = std::make_unique<scene::text>();
	ambience_volume_name_text = std::make_unique<scene::text>();
	ambience_volume_value_text = std::make_unique<scene::text>();
	effects_volume_name_text = std::make_unique<scene::text>();
	effects_volume_value_text = std::make_unique<scene::text>();
	//captions_name_text = std::make_unique<scene::text>();
	//captions_value_text = std::make_unique<scene::text>();
	//captions_size_name_text = std::make_unique<scene::text>();
	//captions_size_value_text = std::make_unique<scene::text>();
	back_text = std::make_unique<scene::text>();
	
	// Build list of menu item texts
	ctx.menu_item_texts.push_back({master_volume_name_text.get(), master_volume_value_text.get()});
	ctx.menu_item_texts.push_back({ambience_volume_name_text.get(), ambience_volume_value_text.get()});
	ctx.menu_item_texts.push_back({effects_volume_name_text.get(), effects_volume_value_text.get()});
	//ctx.menu_item_texts.push_back({captions_name_text.get(), captions_value_text.get()});
	//ctx.menu_item_texts.push_back({captions_size_name_text.get(), captions_size_value_text.get()});
	ctx.menu_item_texts.push_back({back_text.get(), nullptr});
	
	// Set content of menu item texts
	master_volume_name_text->set_content(get_string(ctx, "sound_menu_master_volume"));
	ambience_volume_name_text->set_content(get_string(ctx, "sound_menu_ambience_volume"));
	effects_volume_name_text->set_content(get_string(ctx, "sound_menu_effects_volume"));
	//captions_name_text->set_content(get_string(ctx, "sound_menu_captions"));
	//captions_size_name_text->set_content(get_string(ctx, "sound_menu_captions_size"));
	back_text->set_content(get_string(ctx, "back"));
	update_value_text_content();
	
	// Init menu item index
	::menu::init_menu_item_index(ctx, "sound"_fnv1a32);
	
	::menu::update_text_color(ctx);
	::menu::update_text_font(ctx);
	::menu::align_text(ctx);
	::menu::add_text_to_ui(ctx);
	
	// Construct menu item callbacks

	auto increase_master_volume_callback = [this, &ctx]()
	{
		// Increase volume
		if (ctx.menu_modifier_action.is_active())
			ctx.master_volume += 0.01f;
		else
			ctx.master_volume += 0.1f;

		// Limit volume
		ctx.master_volume = math::clamp(ctx.master_volume, 0.0f, 1.0f);

		// Update listener gain
		ctx.sound_system->get_listener().set_gain(ctx.master_volume);

		// Update master volume setting
		(*ctx.settings)["master_volume"] = ctx.master_volume;

		this->update_value_text_content();
		::menu::align_text(ctx);
	};
	auto decrease_master_volume_callback = [this, &ctx]()
	{
		// Decrease volume
		if (ctx.menu_modifier_action.is_active())
			ctx.master_volume -= 0.01f;
		else
			ctx.master_volume -= 0.1f;

		// Limit volume
		ctx.master_volume = math::clamp(ctx.master_volume, 0.0f, 1.0f);

		// Update listener gain
		ctx.sound_system->get_listener().set_gain(ctx.master_volume);

		// Update master volume setting
		(*ctx.settings)["master_volume"] = ctx.master_volume;

		this->update_value_text_content();
		::menu::align_text(ctx);
	};

	auto increase_ambience_volume_callback = [this, &ctx]()
	{
		// Increase volume
		if (ctx.menu_modifier_action.is_active())
			ctx.ambience_volume += 0.01f;
		else
			ctx.ambience_volume += 0.1f;

		// Limit volume
		ctx.ambience_volume = math::clamp(ctx.ambience_volume, 0.0f, 1.0f);

		// Update volume setting
		(*ctx.settings)["ambience_volume"] = ctx.ambience_volume;

		this->update_value_text_content();
		::menu::align_text(ctx);
	};
	auto decrease_ambience_volume_callback = [this, &ctx]()
	{
		// Decrease volume
		if (ctx.menu_modifier_action.is_active())
			ctx.ambience_volume -= 0.01f;
		else
			ctx.ambience_volume -= 0.1f;

		// Limit volume
		ctx.ambience_volume = math::clamp(ctx.ambience_volume, 0.0f, 1.0f);

		// Update volume setting
		(*ctx.settings)["ambience_volume"] = ctx.ambience_volume;

		this->update_value_text_content();
		::menu::align_text(ctx);
	};

	auto increase_effects_volume_callback = [this, &ctx]()
	{
		// Increase volume
		if (ctx.menu_modifier_action.is_active())
			ctx.effects_volume += 0.01f;
		else
			ctx.effects_volume += 0.1f;

		// Limit volume
		ctx.effects_volume = math::clamp(ctx.effects_volume, 0.0f, 1.0f);

		// Update volume setting
		(*ctx.settings)["effects_volume"] = ctx.effects_volume;

		this->update_value_text_content();
		::menu::align_text(ctx);
	};
	auto decrease_effects_volume_callback = [this, &ctx]()
	{
		// Decrease volume
		if (ctx.menu_modifier_action.is_active())
			ctx.effects_volume -= 0.01f;
		else
			ctx.effects_volume -= 0.1f;

		// Limit volume
		ctx.effects_volume = math::clamp(ctx.effects_volume, 0.0f, 1.0f);

		// Update volume setting
		(*ctx.settings)["effects_volume"] = ctx.effects_volume;

		this->update_value_text_content();
		::menu::align_text(ctx);
	};
	
	/*
	auto toggle_captions_callback = [this, &ctx]()
	{
		ctx.captions = !ctx.captions;
		
		this->update_value_text_content();
		::menu::align_text(ctx);
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
	};
	*/

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
	ctx.menu_select_callbacks.push_back(std::bind(increase_master_volume_callback));
	ctx.menu_select_callbacks.push_back(std::bind(increase_ambience_volume_callback));
	ctx.menu_select_callbacks.push_back(std::bind(increase_effects_volume_callback));
	//ctx.menu_select_callbacks.push_back(toggle_captions_callback);
	//ctx.menu_select_callbacks.push_back(increase_captions_size_callback);
	ctx.menu_select_callbacks.push_back(select_back_callback);
	
	// Build list of menu left callbacks
	ctx.menu_left_callbacks.push_back(std::bind(decrease_master_volume_callback));
	ctx.menu_left_callbacks.push_back(std::bind(decrease_ambience_volume_callback));
	ctx.menu_left_callbacks.push_back(std::bind(decrease_effects_volume_callback));
	//ctx.menu_left_callbacks.push_back(toggle_captions_callback);
	//ctx.menu_left_callbacks.push_back(decrease_captions_size_callback);
	ctx.menu_left_callbacks.push_back(nullptr);
	
	// Build list of menu right callbacks
	ctx.menu_right_callbacks.push_back(std::bind(increase_master_volume_callback));
	ctx.menu_right_callbacks.push_back(std::bind(increase_ambience_volume_callback));
	ctx.menu_right_callbacks.push_back(std::bind(increase_effects_volume_callback));
	//ctx.menu_right_callbacks.push_back(toggle_captions_callback);
	//ctx.menu_right_callbacks.push_back(increase_captions_size_callback);
	ctx.menu_right_callbacks.push_back(nullptr);
	
	// Set menu back callback
	ctx.menu_back_callback = select_back_callback;
	
	// Queue menu control setup
	ctx.function_queue.push(std::bind(::enable_menu_controls, std::ref(ctx)));
	
	// Fade in menu
	::menu::fade_in(ctx, nullptr);
	
	debug::log_trace("Entered sound menu state");
}

sound_menu_state::~sound_menu_state()
{
	debug::log_trace("Exiting sound menu state...");
	
	// Destruct menu
	::disable_menu_controls(ctx);
	::menu::clear_callbacks(ctx);
	::menu::remove_text_from_ui(ctx);
	::menu::delete_text(ctx);
	
	debug::log_trace("Exited sound menu state");
}

void sound_menu_state::update_value_text_content()
{
	const std::string string_on = get_string(ctx, "on");
	const std::string string_off = get_string(ctx, "off");
	
	std::get<1>(ctx.menu_item_texts[0])->set_content(std::to_string(static_cast<int>(std::round(ctx.master_volume * 100.0f))) + "%");
	std::get<1>(ctx.menu_item_texts[1])->set_content(std::to_string(static_cast<int>(std::round(ctx.ambience_volume * 100.0f))) + "%");
	std::get<1>(ctx.menu_item_texts[2])->set_content(std::to_string(static_cast<int>(std::round(ctx.effects_volume * 100.0f))) + "%");
	//std::get<1>(ctx.menu_item_texts[4])->set_content((ctx.captions) ? string_on : string_off);
	//std::get<1>(ctx.menu_item_texts[5])->set_content(std::to_string(static_cast<int>(std::round(ctx.captions_size * 100.0f))) + "%");
}

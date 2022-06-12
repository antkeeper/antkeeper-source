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
#include "render/passes/clear-pass.hpp"
#include "debug/logger.hpp"

namespace game {
namespace state {
namespace sound_menu {

static void update_text_color(game::context* ctx)
{
	float4 inactive_color = {1.0f, 1.0f, 1.0f, 0.5f};
	float4 active_color = {1.0f, 1.0f, 1.0f, 1.0f};
	
	for (std::size_t i = 0; i < ctx->sound_menu_label_texts.size(); ++i)
	{
		scene::text* label_text = ctx->sound_menu_label_texts[i];
		scene::text* value_text = ctx->sound_menu_value_texts[i];

		if (i == ctx->sound_menu_index)
		{
			label_text->set_color(active_color);
			if (value_text)
				value_text->set_color(active_color);
		}
		else
		{
			label_text->set_color(inactive_color);
			if (value_text)
				value_text->set_color(inactive_color);
		}
	}
}

static void update_text_tweens(game::context* ctx)
{
	for (std::size_t i = 0; i < ctx->sound_menu_label_texts.size(); ++i)
	{
		scene::text* label_text = ctx->sound_menu_label_texts[i];
		scene::text* value_text = ctx->sound_menu_value_texts[i];
		
		label_text->update_tweens();
		if (value_text)
			value_text->update_tweens();
	}
}

void enter(game::context* ctx)
{
	ctx->ui_clear_pass->set_cleared_buffers(true, true, false);
	
	// Construct sound menu texts
	ctx->sound_menu_master_volume_label_text = new scene::text();
	ctx->sound_menu_master_volume_value_text = new scene::text();
	ctx->sound_menu_ambience_volume_label_text = new scene::text();
	ctx->sound_menu_ambience_volume_value_text = new scene::text();
	ctx->sound_menu_effects_volume_label_text = new scene::text();
	ctx->sound_menu_effects_volume_value_text = new scene::text();
	ctx->sound_menu_back_label_text = new scene::text();
	
	// Set text content
	ctx->sound_menu_master_volume_label_text->set_content((*ctx->strings)["sound_menu_master_volume"]);
	ctx->sound_menu_master_volume_value_text->set_content(std::to_string(static_cast<int>(ctx->master_volume * 100.0f + 0.5f)) + "%");
	ctx->sound_menu_ambience_volume_label_text->set_content((*ctx->strings)["sound_menu_ambience_volume"]);
	ctx->sound_menu_ambience_volume_value_text->set_content(std::to_string(static_cast<int>(ctx->ambience_volume * 100.0f + 0.5f)) + "%");
	ctx->sound_menu_effects_volume_label_text->set_content((*ctx->strings)["sound_menu_effects_volume"]);
	ctx->sound_menu_effects_volume_value_text->set_content(std::to_string(static_cast<int>(ctx->effects_volume * 100.0f + 0.5f)) + "%");
	ctx->sound_menu_back_label_text->set_content((*ctx->strings)["back"]);
	
	// Build lists of sound menu texts
	ctx->sound_menu_label_texts.push_back(ctx->sound_menu_master_volume_label_text);
	ctx->sound_menu_label_texts.push_back(ctx->sound_menu_ambience_volume_label_text);
	ctx->sound_menu_label_texts.push_back(ctx->sound_menu_effects_volume_label_text);
	ctx->sound_menu_label_texts.push_back(ctx->sound_menu_back_label_text);
	ctx->sound_menu_value_texts.push_back(ctx->sound_menu_master_volume_value_text);
	ctx->sound_menu_value_texts.push_back(ctx->sound_menu_ambience_volume_value_text);
	ctx->sound_menu_value_texts.push_back(ctx->sound_menu_effects_volume_value_text);
	ctx->sound_menu_value_texts.push_back(nullptr);
	
	// Set text fonts
	for (std::size_t i = 0; i < ctx->sound_menu_label_texts.size(); ++i)
	{
		scene::text* label_text = ctx->sound_menu_label_texts[i];
		label_text->set_material(&ctx->menu_font_material);
		label_text->set_font(&ctx->menu_font);
		
		scene::text* value_text = ctx->sound_menu_value_texts[i];
		if (value_text)
		{
			value_text->set_material(&ctx->menu_font_material);
			value_text->set_font(&ctx->menu_font);
		}
	}
	
	// Calculate menu width
	float menu_width = 0.0f;
	float menu_spacing = ctx->menu_font.get_glyph_metrics(U'M').width;
	for (std::size_t i = 0; i < ctx->sound_menu_label_texts.size(); ++i)
	{
		scene::text* label_text = ctx->sound_menu_label_texts[i];
		scene::text* value_text = ctx->sound_menu_value_texts[i];
		
		float row_width = 0.0f;
		
		// Add label width to width
		const auto& label_bounds = static_cast<const geom::aabb<float>&>(label_text->get_local_bounds());
		row_width += label_bounds.max_point.x - label_bounds.min_point.x;
		
		if (value_text != nullptr)
		{
			// Add value width to width
			//const auto& value_bounds = static_cast<const geom::aabb<float>&>(value_text->get_local_bounds());
			//row_width += value_bounds.max_point.x - value_bounds.min_point.x;
			
			// Add spacing to row width
			row_width += menu_spacing * 4.0f;
		}
		
		menu_width = std::max<float>(menu_width, row_width);
	}
	
	// Align texts
	float menu_height = ctx->sound_menu_label_texts.size() * ctx->menu_font.get_font_metrics().linespace;
	float menu_x = -menu_width * 0.5f;
	float menu_y = menu_height * 0.5f - ctx->menu_font.get_font_metrics().linespace;
	for (std::size_t i = 0; i < ctx->sound_menu_label_texts.size(); ++i)
	{
		scene::text* label_text = ctx->sound_menu_label_texts[i];
		
		float x = menu_x;
		float y = menu_y - ctx->menu_font.get_font_metrics().linespace * i;
		label_text->set_translation({std::round(x), std::round(y), 0.0f});
		
		scene::text* value_text = ctx->sound_menu_value_texts[i];
		if (value_text)
		{
			const auto& value_bounds = static_cast<const geom::aabb<float>&>(value_text->get_local_bounds());
			const float value_width =  value_bounds.max_point.x - value_bounds.min_point.x;
			x = menu_x + menu_width - value_width;
			
			value_text->set_translation({std::round(x), std::round(y), 0.0f});
		}
	}
	
	// Construct sound menu callbacks
	auto menu_back_callback = [ctx]()
	{
		application::state next_state;
		next_state.name = "options_menu";
		next_state.enter = std::bind(game::state::options_menu::enter, ctx);
		next_state.exit = std::bind(game::state::options_menu::exit, ctx);
		ctx->app->change_state(next_state);
	};
	auto increase_volume_callback = [ctx, menu_x, menu_width]()
	{
		if (ctx->sound_menu_index > 2)
			return;
		
		// Find volume variable
		float* volumes[] =
		{
			&ctx->master_volume,
			&ctx->ambience_volume,
			&ctx->effects_volume
		};
		float* volume = volumes[ctx->sound_menu_index];
		
		// Increase volume
		if (ctx->controls["menu_modifier"]->is_active())
			*volume += 0.01f;
		else
			*volume += 0.1f;
		
		// Limit volume
		if (*volume > 1.0f)
			*volume = 1.0f;
		
		// Update volume value text
		scene::text* value_text = ctx->sound_menu_value_texts[ctx->sound_menu_index];
		value_text->set_content(std::to_string(static_cast<int>((*volume) * 100.0f + 0.5f)) + "%");
		
		// Realign value text
		const auto& value_bounds = static_cast<const geom::aabb<float>&>(value_text->get_local_bounds());
		const float value_width =  value_bounds.max_point.x - value_bounds.min_point.x;
		const float3& translation = value_text->get_translation();
		value_text->set_translation(float3{std::round(menu_x + menu_width - value_width), translation.y, translation.z});
		value_text->update_tweens();
	};
	auto decrease_volume_callback = [ctx, menu_x, menu_width]()
	{
		if (ctx->sound_menu_index > 2)
			return;
		
		// Find volume variable
		float* volumes[] =
		{
			&ctx->master_volume,
			&ctx->ambience_volume,
			&ctx->effects_volume
		};
		float* volume = volumes[ctx->sound_menu_index];
		
		// Decrease volume
		if (ctx->controls["menu_modifier"]->is_active())
			*volume -= 0.01f;
		else
			*volume -= 0.1f;
		
		// Limit volume
		if (*volume < 0.0f)
			*volume = 0.0f;
		
		// Update volume value text
		scene::text* value_text = ctx->sound_menu_value_texts[ctx->sound_menu_index];
		value_text->set_content(std::to_string(static_cast<int>((*volume) * 100.0f + 0.5f)) + "%");
		
		// Realign value text
		const auto& value_bounds = static_cast<const geom::aabb<float>&>(value_text->get_local_bounds());
		const float value_width =  value_bounds.max_point.x - value_bounds.min_point.x;
		const float3& translation = value_text->get_translation();
		value_text->set_translation(float3{std::round(menu_x + menu_width - value_width), translation.y, translation.z});
		value_text->update_tweens();
	};
	
	ctx->controls["menu_down"]->set_activated_callback
	(
		[ctx]()
		{
			++ctx->sound_menu_index;
			if (ctx->sound_menu_index >= ctx->sound_menu_label_texts.size())
				ctx->sound_menu_index = 0;
			
			update_text_color(ctx);
		}
	);
	ctx->controls["menu_up"]->set_activated_callback
	(
		[ctx]()
		{
			--ctx->sound_menu_index;
			if (ctx->sound_menu_index < 0)
				ctx->sound_menu_index = ctx->sound_menu_label_texts.size() - 1;
			
			update_text_color(ctx);
		}
	);
	ctx->controls["menu_left"]->set_activated_callback(decrease_volume_callback);
	ctx->controls["menu_right"]->set_activated_callback(increase_volume_callback);
	ctx->controls["menu_select"]->set_activated_callback
	(
		[ctx, menu_back_callback]()
		{
			if (ctx->sound_menu_index == 3)
				menu_back_callback();
		}
	);
	ctx->controls["menu_back"]->set_activated_callback(menu_back_callback);
	
	// Add text objects to UI
	for (std::size_t i = 0; i < ctx->sound_menu_label_texts.size(); ++i)
	{
		scene::text* label_text = ctx->sound_menu_label_texts[i];
		scene::text* value_text = ctx->sound_menu_value_texts[i];
		
		ctx->ui_scene->add_object(label_text);
		if (value_text)
			ctx->ui_scene->add_object(value_text);
	}
	update_text_color(ctx);
	update_text_tweens(ctx);
}

void exit(game::context* ctx)
{
	// Clear control callbacks
	ctx->controls["menu_down"]->set_activated_callback(nullptr);
	ctx->controls["menu_up"]->set_activated_callback(nullptr);
	ctx->controls["menu_left"]->set_activated_callback(nullptr);
	ctx->controls["menu_right"]->set_activated_callback(nullptr);
	ctx->controls["menu_select"]->set_activated_callback(nullptr);
	ctx->controls["menu_back"]->set_activated_callback(nullptr);
	
	// Destruct sound menu texts
	for (std::size_t i = 0; i < ctx->sound_menu_label_texts.size(); ++i)
	{
		scene::text* label_text = ctx->sound_menu_label_texts[i];
		ctx->ui_scene->remove_object(label_text);
		delete label_text;
		
		scene::text* value_text = ctx->sound_menu_value_texts[i];
		if (value_text)
		{
			ctx->ui_scene->remove_object(value_text);
			delete value_text;
		}
	}
	ctx->sound_menu_label_texts.clear();
	ctx->sound_menu_value_texts.clear();
	
	// Update volumes in config
	(*ctx->config)["master_volume"] = ctx->master_volume;
	(*ctx->config)["ambience_volume"] = ctx->ambience_volume;
	(*ctx->config)["effects_volume"] = ctx->effects_volume;
	
	ctx->ui_clear_pass->set_cleared_buffers(false, true, false);
}

} // namespace sound_menu
} // namespace state
} // namespace game

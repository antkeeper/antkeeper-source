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

#include "game/states/options-menu.hpp"
#include "game/states/main-menu.hpp"
#include "game/states/language-menu.hpp"
#include "animation/ease.hpp"
#include "animation/animation.hpp"
#include "animation/animator.hpp"
#include "application.hpp"
#include "scene/text.hpp"
#include "render/passes/clear-pass.hpp"

namespace game {
namespace state {
namespace options_menu {

void enter(game::context* ctx)
{
	ctx->ui_clear_pass->set_cleared_buffers(true, true, false);
	
	ctx->options_menu_index = 0;
	
	// Construct options menu texts
	ctx->options_menu_controls_text = new scene::text();
	ctx->options_menu_graphics_text = new scene::text();
	ctx->options_menu_sound_text = new scene::text();
	ctx->options_menu_language_text = new scene::text();
	ctx->options_menu_back_text = new scene::text();
	
	// Build list of options menu texts
	ctx->options_menu_texts.push_back(ctx->options_menu_controls_text);
	ctx->options_menu_texts.push_back(ctx->options_menu_graphics_text);
	ctx->options_menu_texts.push_back(ctx->options_menu_sound_text);
	ctx->options_menu_texts.push_back(ctx->options_menu_language_text);
	ctx->options_menu_texts.push_back(ctx->options_menu_back_text);
	
	// Construct options menu callbacks
	auto menu_back_callback = [ctx]()
	{
		application::state next_state;
		next_state.name = "main_menu";
		next_state.enter = std::bind(game::state::main_menu::enter, ctx, 1);
		next_state.exit = std::bind(game::state::main_menu::exit, ctx);
		ctx->app->change_state(next_state);
	};
	auto change_state_language_menu = [ctx]()
	{
		application::state next_state;
		next_state.name = "language_menu";
		next_state.enter = std::bind(game::state::language_menu::enter, ctx);
		next_state.exit = std::bind(game::state::language_menu::exit, ctx);
		ctx->app->change_state(next_state);
	};
	
	// Build list of options menu callbacks
	ctx->options_menu_callbacks.push_back(nullptr);
	ctx->options_menu_callbacks.push_back(nullptr);
	ctx->options_menu_callbacks.push_back(nullptr);
	ctx->options_menu_callbacks.push_back(change_state_language_menu);
	ctx->options_menu_callbacks.push_back(menu_back_callback);
	
	// Set content of texts
	ctx->options_menu_controls_text->set_content((*ctx->strings)["options_menu_controls"]);
	ctx->options_menu_graphics_text->set_content((*ctx->strings)["options_menu_graphics"]);
	ctx->options_menu_sound_text->set_content((*ctx->strings)["options_menu_sound"]);
	ctx->options_menu_language_text->set_content((*ctx->strings)["options_menu_language"]);
	ctx->options_menu_back_text->set_content((*ctx->strings)["back"]);
	
	float4 inactive_color = {1.0f, 1.0f, 1.0f, 0.5f};
	float4 active_color = {1.0f, 1.0f, 1.0f, 1.0f};
	float menu_width = 0.0f;
	for (std::size_t i = 0; i < ctx->options_menu_texts.size(); ++i)
	{
		scene::text* text = ctx->options_menu_texts[i];
		
		// Set text material and font
		text->set_material(&ctx->menu_font_material);
		text->set_font(&ctx->menu_font);
		
		// Set text color
		if (i == ctx->options_menu_index)
			text->set_color(active_color);
		else
			text->set_color(inactive_color);
		
		// Update menu width
		const auto& bounds = static_cast<const geom::aabb<float>&>(text->get_local_bounds());
		float width = bounds.max_point.x - bounds.min_point.x;
		menu_width = std::max<float>(menu_width, width);
		
		// Add text to UI
		ctx->ui_scene->add_object(text);
	}
	
	// Align texts
	float menu_height = ctx->options_menu_texts.size() * ctx->menu_font.get_font_metrics().linespace;
	float menu_x = -menu_width * 0.5f;
	float menu_y = menu_height * 0.5f - ctx->menu_font.get_font_metrics().linespace;
	for (std::size_t i = 0; i < ctx->options_menu_texts.size(); ++i)
	{
		scene::text* text = ctx->options_menu_texts[i];
		
		float x = menu_x;
		float y = menu_y - ctx->menu_font.get_font_metrics().linespace * i;
		
		text->set_translation({std::round(x), std::round(y), 0.0f});
		text->update_tweens();
	}
	
	ctx->controls["menu_down"]->set_activated_callback
	(
		[ctx]()
		{
			++ctx->options_menu_index;
			if (ctx->options_menu_index >= ctx->options_menu_texts.size())
				ctx->options_menu_index = 0;
			
			float4 active_color{1.0f, 1.0f, 1.0f, 1.0f};
			float4 inactive_color{1.0f, 1.0f, 1.0f, 0.5f};
			
			for (std::size_t i = 0; i < ctx->options_menu_texts.size(); ++i)
			{
				scene::text* text = ctx->options_menu_texts[i];
				
				if (i == ctx->options_menu_index)
					text->set_color(active_color);
				else
					text->set_color(inactive_color);
			}
		}
	);
	ctx->controls["menu_up"]->set_activated_callback
	(
		[ctx]()
		{
			--ctx->options_menu_index;
			if (ctx->options_menu_index < 0)
				ctx->options_menu_index = ctx->options_menu_texts.size() - 1;
			
			float4 active_color{1.0f, 1.0f, 1.0f, 1.0f};
			float4 inactive_color{1.0f, 1.0f, 1.0f, 0.5f};
			
			for (std::size_t i = 0; i < ctx->options_menu_texts.size(); ++i)
			{
				scene::text* text = ctx->options_menu_texts[i];
				
				if (i == ctx->options_menu_index)
					text->set_color(active_color);
				else
					text->set_color(inactive_color);
			}
		}
	);
	ctx->controls["menu_select"]->set_activated_callback
	(
		[ctx]()
		{
			auto callback = ctx->options_menu_callbacks[ctx->options_menu_index];
			if (callback != nullptr)
				callback();
		}
	);
	ctx->controls["menu_back"]->set_activated_callback(menu_back_callback);
	/*
	ctx->controls["menu_back"]->set_activated_callback
	(
		std::bind(&application::close, ctx->app, 0)
	);
	*/
}

void exit(game::context* ctx)
{
	// Clear control callbacks
	ctx->controls["menu_down"]->set_activated_callback(nullptr);
	ctx->controls["menu_up"]->set_activated_callback(nullptr);
	ctx->controls["menu_select"]->set_activated_callback(nullptr);
	ctx->controls["menu_back"]->set_activated_callback(nullptr);
	
	// Clear options menu callbacks
	ctx->options_menu_callbacks.clear();
	
	// Destruct options menu texts
	for (scene::text* text: ctx->options_menu_texts)
	{
		ctx->ui_scene->remove_object(text);
		delete text;
	}
	ctx->options_menu_texts.clear();
	
	ctx->ui_clear_pass->set_cleared_buffers(false, true, false);
}

} // namespace options_menu
} // namespace state
} // namespace game

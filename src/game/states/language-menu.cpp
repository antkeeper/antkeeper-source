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

#include "game/states/language-menu.hpp"
#include "game/states/options-menu.hpp"
#include "application.hpp"
#include "scene/text.hpp"
#include "render/passes/clear-pass.hpp"
#include "debug/logger.hpp"
#include "game/fonts.hpp"

namespace game {
namespace state {
namespace language_menu {

static void update_text_font(game::context* ctx)
{
	for (scene::text* text: ctx->language_menu_texts)
	{
		text->set_material(&ctx->menu_font_material);
		text->set_font(&ctx->menu_font);
	}
}

static void update_text_color(game::context* ctx)
{
	float4 inactive_color = {1.0f, 1.0f, 1.0f, 0.5f};
	float4 active_color = {1.0f, 1.0f, 1.0f, 1.0f};
	
	for (std::size_t i = 0; i < ctx->language_menu_texts.size(); ++i)
	{
		scene::text* text = ctx->language_menu_texts[i];
		
		if (i == ctx->language_menu_index)
			text->set_color(active_color);
		else
			text->set_color(inactive_color);
	}
}

static void update_text_content(game::context* ctx)
{
	ctx->language_menu_language_text->set_content((*ctx->strings)["language_name"]);
	ctx->language_menu_back_text->set_content((*ctx->strings)["back"]);
}

static void refresh_texts(game::context* ctx)
{
	for (scene::text* text: ctx->language_menu_texts)
	{
		text->refresh();
	}
}

static void align_texts(game::context* ctx)
{
	float menu_width = 0.0f;
	for (std::size_t i = 0; i < ctx->language_menu_texts.size(); ++i)
	{
		scene::text* text = ctx->language_menu_texts[i];
		
		// Update menu width
		const auto& bounds = static_cast<const geom::aabb<float>&>(text->get_local_bounds());
		float width = bounds.max_point.x - bounds.min_point.x;
		menu_width = std::max<float>(menu_width, width);
	}
	
	float menu_height = ctx->language_menu_texts.size() * ctx->menu_font.get_font_metrics().linespace;
	float menu_x = -menu_width * 0.5f;
	float menu_y = menu_height * 0.5f - ctx->menu_font.get_font_metrics().linespace;
	for (std::size_t i = 0; i < ctx->language_menu_texts.size(); ++i)
	{
		scene::text* text = ctx->language_menu_texts[i];
		
		// Align text
		const auto& bounds = static_cast<const geom::aabb<float>&>(text->get_local_bounds());
		float w = bounds.max_point.x - bounds.min_point.x;
		float x = -w * 0.5f;		
		//float x = menu_x;
		float y = menu_y - ctx->menu_font.get_font_metrics().linespace * i;
		
		text->set_translation({std::round(x), std::round(y), 0.0f});
	}
}

static void update_text_tweens(game::context* ctx)
{
	for (scene::text* text: ctx->language_menu_texts)
	{
		text->update_tweens();
	}
}

void enter(game::context* ctx)
{
	ctx->ui_clear_pass->set_cleared_buffers(true, true, false);
	
	// Construct language menu texts
	ctx->language_menu_language_text = new scene::text();
	ctx->language_menu_back_text = new scene::text();
	
	// Build list of language menu texts
	ctx->language_menu_texts.push_back(ctx->language_menu_language_text);
	ctx->language_menu_texts.push_back(ctx->language_menu_back_text);
	
	// Construct language menu callbacks
	auto menu_back_callback = [ctx]()
	{
		application::state next_state;
		next_state.name = "options_menu";
		next_state.enter = std::bind(game::state::options_menu::enter, ctx);
		next_state.exit = std::bind(game::state::options_menu::exit, ctx);
		ctx->app->change_state(next_state);
	};
	auto next_language_callback = [ctx]()
	{
		if (ctx->language_menu_index != 0)
			return;
		
		// Increment language index
		++ctx->language_index;
		if (ctx->language_index >= ctx->language_count)
			ctx->language_index = 0;
		
		// Find corresponding language code and strings
		ctx->language_code = (*ctx->string_table)[0][ctx->language_index + 2];
		ctx->strings = &ctx->string_table_map[ctx->language_code];
		
		// Update language in config
		(*ctx->config)["language"] = ctx->language_code;
		
		ctx->logger->log("Language changed to \"" + ctx->language_code + "\"");
		
		// Reload fonts
		ctx->logger->push_task("Reloading fonts");
		try
		{
			game::load_fonts(ctx);
		}
		catch (...)
		{
			ctx->logger->pop_task(EXIT_FAILURE);
		}
		ctx->logger->pop_task(EXIT_SUCCESS);
		
		update_text_font(ctx);
		update_text_content(ctx);
		refresh_texts(ctx);
		align_texts(ctx);
		update_text_tweens(ctx);
	};
	auto previous_language_callback = [ctx]()
	{
		if (ctx->language_menu_index != 0)
			return;
		
		// Increment language index
		--ctx->language_index;
		if (ctx->language_index < 0)
			ctx->language_index = ctx->language_count - 1;
		
		// Find corresponding language code and strings
		ctx->language_code = (*ctx->string_table)[0][ctx->language_index + 2];
		ctx->strings = &ctx->string_table_map[ctx->language_code];
		
		// Update language in config
		(*ctx->config)["language"] = ctx->language_code;
		
		ctx->logger->log("Language changed to \"" + ctx->language_code + "\"");
		
		// Reload fonts
		ctx->logger->push_task("Reloading fonts");
		try
		{
			game::load_fonts(ctx);
		}
		catch (...)
		{
			ctx->logger->pop_task(EXIT_FAILURE);
		}
		ctx->logger->pop_task(EXIT_SUCCESS);
		
		update_text_font(ctx);
		update_text_content(ctx);
		refresh_texts(ctx);
		align_texts(ctx);
		update_text_tweens(ctx);
	};
	
	// Build list of language menu callbacks
	ctx->language_menu_callbacks.push_back(next_language_callback);
	ctx->language_menu_callbacks.push_back(menu_back_callback);
	
	ctx->controls["menu_down"]->set_activated_callback
	(
		[ctx]()
		{
			++ctx->language_menu_index;
			if (ctx->language_menu_index >= ctx->language_menu_texts.size())
				ctx->language_menu_index = 0;
			
			update_text_color(ctx);
		}
	);
	ctx->controls["menu_up"]->set_activated_callback
	(
		[ctx]()
		{
			--ctx->language_menu_index;
			if (ctx->language_menu_index < 0)
				ctx->language_menu_index = ctx->language_menu_texts.size() - 1;
			
			update_text_color(ctx);
		}
	);
	ctx->controls["menu_left"]->set_activated_callback(previous_language_callback);
	ctx->controls["menu_right"]->set_activated_callback(next_language_callback);
	ctx->controls["menu_select"]->set_activated_callback
	(
		[ctx]()
		{
			auto callback = ctx->language_menu_callbacks[ctx->language_menu_index];
			if (callback != nullptr)
				callback();
		}
	);
	ctx->controls["menu_back"]->set_activated_callback(menu_back_callback);
	
	for (scene::text* text: ctx->language_menu_texts)
		ctx->ui_scene->add_object(text);
	update_text_font(ctx);
	update_text_color(ctx);
	update_text_content(ctx);
	align_texts(ctx);
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
	
	// Clear language menu callbacks
	ctx->language_menu_callbacks.clear();
	
	// Destruct language menu texts
	for (scene::text* text: ctx->language_menu_texts)
	{
		ctx->ui_scene->remove_object(text);
		delete text;
	}
	ctx->language_menu_texts.clear();
	
	ctx->ui_clear_pass->set_cleared_buffers(false, true, false);
}

} // namespace language_menu
} // namespace state
} // namespace game

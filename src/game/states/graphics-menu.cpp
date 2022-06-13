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

#include "game/states/graphics-menu.hpp"
#include "game/states/options-menu.hpp"
#include "application.hpp"
#include "scene/text.hpp"
#include "render/passes/clear-pass.hpp"
#include "debug/logger.hpp"
#include "game/fonts.hpp"

namespace game {
namespace state {
namespace graphics_menu {

static void update_text_color(game::context* ctx)
{
	float4 inactive_color = {1.0f, 1.0f, 1.0f, 0.5f};
	float4 active_color = {1.0f, 1.0f, 1.0f, 1.0f};
	
	for (std::size_t i = 0; i < ctx->graphics_menu_label_texts.size(); ++i)
	{
		scene::text* label_text = ctx->graphics_menu_label_texts[i];
		scene::text* value_text = ctx->graphics_menu_value_texts[i];

		if (i == ctx->graphics_menu_index)
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

static void align_texts(game::context* ctx)
{
	// Calculate menu width
	float menu_width = 0.0f;
	float menu_spacing = ctx->menu_font.get_glyph_metrics(U'M').width;
	for (std::size_t i = 0; i < ctx->graphics_menu_label_texts.size(); ++i)
	{
		scene::text* label_text = ctx->graphics_menu_label_texts[i];
		scene::text* value_text = ctx->graphics_menu_value_texts[i];
		
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
			row_width += menu_spacing * 8.0f;
		}
		
		menu_width = std::max<float>(menu_width, row_width);
	}
	
	// Align texts
	float menu_height = ctx->graphics_menu_label_texts.size() * ctx->menu_font.get_font_metrics().linespace;
	float menu_x = -menu_width * 0.5f;
	float menu_y = menu_height * 0.5f - ctx->menu_font.get_font_metrics().linespace;
	for (std::size_t i = 0; i < ctx->graphics_menu_label_texts.size(); ++i)
	{
		scene::text* label_text = ctx->graphics_menu_label_texts[i];
		
		float x = menu_x;
		float y = menu_y - ctx->menu_font.get_font_metrics().linespace * i;
		label_text->set_translation({std::round(x), std::round(y), 0.0f});
		
		scene::text* value_text = ctx->graphics_menu_value_texts[i];
		if (value_text)
		{
			const auto& value_bounds = static_cast<const geom::aabb<float>&>(value_text->get_local_bounds());
			const float value_width =  value_bounds.max_point.x - value_bounds.min_point.x;
			x = menu_x + menu_width - value_width;
			
			value_text->set_translation({std::round(x), std::round(y), 0.0f});
		}
	}
}

static void update_text_tweens(game::context* ctx)
{
	for (std::size_t i = 0; i < ctx->graphics_menu_label_texts.size(); ++i)
	{
		scene::text* label_text = ctx->graphics_menu_label_texts[i];
		scene::text* value_text = ctx->graphics_menu_value_texts[i];
		
		label_text->update_tweens();
		if (value_text)
			value_text->update_tweens();
	}
}

static void refresh_texts(game::context* ctx)
{
	for (std::size_t i = 0; i < ctx->graphics_menu_label_texts.size(); ++i)
	{
		scene::text* label_text = ctx->graphics_menu_label_texts[i];
		label_text->refresh();
		
		scene::text* value_text = ctx->graphics_menu_value_texts[i];
		if (value_text)
			value_text->refresh();
	}
}

void enter(game::context* ctx)
{
	ctx->ui_clear_pass->set_cleared_buffers(true, true, false);
	
	// Construct graphics menu texts
	ctx->graphics_menu_display_mode_label_text = new scene::text();
	ctx->graphics_menu_display_mode_value_text = new scene::text();
	ctx->graphics_menu_render_resolution_label_text = new scene::text();
	ctx->graphics_menu_render_resolution_value_text = new scene::text();
	ctx->graphics_menu_v_sync_label_text = new scene::text();
	ctx->graphics_menu_v_sync_value_text = new scene::text();
	ctx->graphics_menu_font_size_label_text = new scene::text();
	ctx->graphics_menu_font_size_value_text = new scene::text();
	ctx->graphics_menu_dyslexia_font_label_text = new scene::text();
	ctx->graphics_menu_dyslexia_font_value_text = new scene::text();
	ctx->graphics_menu_back_label_text = new scene::text();
	
	bool fullscreen = ctx->app->is_fullscreen();
	float render_resolution = ctx->render_resolution_scale;
	bool vsync = ctx->app->get_vsync();
	float font_size = ctx->font_size;
	bool dyslexia_font = ctx->dyslexia_font;
	
	const std::string string_fullscreen = (*ctx->strings)["graphics_menu_display_mode_fullscreen"];
	const std::string string_window = (*ctx->strings)["graphics_menu_display_mode_window"];
	const std::string string_on = (*ctx->strings)["on"];
	const std::string string_off = (*ctx->strings)["off"];
	
	// Set text content
	ctx->graphics_menu_display_mode_label_text->set_content((*ctx->strings)["graphics_menu_display_mode"]);
	ctx->graphics_menu_display_mode_value_text->set_content((fullscreen) ? string_fullscreen : string_window);
	ctx->graphics_menu_render_resolution_label_text->set_content((*ctx->strings)["graphics_menu_render_resolution"]);
	ctx->graphics_menu_render_resolution_value_text->set_content(std::to_string(static_cast<int>(std::round(render_resolution * 100.0f))) + "%");
	ctx->graphics_menu_v_sync_label_text->set_content((*ctx->strings)["graphics_menu_v_sync"]);
	ctx->graphics_menu_v_sync_value_text->set_content((vsync) ? string_on : string_off);
	
	ctx->graphics_menu_font_size_label_text->set_content((*ctx->strings)["graphics_menu_font_size"]);
	ctx->graphics_menu_font_size_value_text->set_content(std::to_string(static_cast<int>(std::round(font_size * 100.0f))) + "%");
	
	ctx->graphics_menu_dyslexia_font_label_text->set_content((*ctx->strings)["graphics_menu_dyslexia_font"]);
	ctx->graphics_menu_dyslexia_font_value_text->set_content((dyslexia_font) ? string_on : string_off);
	
	ctx->graphics_menu_back_label_text->set_content((*ctx->strings)["back"]);
	
	// Build lists of graphics menu texts
	ctx->graphics_menu_label_texts.push_back(ctx->graphics_menu_display_mode_label_text);
	ctx->graphics_menu_label_texts.push_back(ctx->graphics_menu_render_resolution_label_text);
	ctx->graphics_menu_label_texts.push_back(ctx->graphics_menu_v_sync_label_text);
	ctx->graphics_menu_label_texts.push_back(ctx->graphics_menu_font_size_label_text);
	ctx->graphics_menu_label_texts.push_back(ctx->graphics_menu_dyslexia_font_label_text);
	ctx->graphics_menu_label_texts.push_back(ctx->graphics_menu_back_label_text);
	ctx->graphics_menu_value_texts.push_back(ctx->graphics_menu_display_mode_value_text);
	ctx->graphics_menu_value_texts.push_back(ctx->graphics_menu_render_resolution_value_text);
	ctx->graphics_menu_value_texts.push_back(ctx->graphics_menu_v_sync_value_text);
	ctx->graphics_menu_value_texts.push_back(ctx->graphics_menu_font_size_value_text);
	ctx->graphics_menu_value_texts.push_back(ctx->graphics_menu_dyslexia_font_value_text);
	ctx->graphics_menu_value_texts.push_back(nullptr);
	
	// Set text fonts
	for (std::size_t i = 0; i < ctx->graphics_menu_label_texts.size(); ++i)
	{
		scene::text* label_text = ctx->graphics_menu_label_texts[i];
		label_text->set_material(&ctx->menu_font_material);
		label_text->set_font(&ctx->menu_font);
		
		scene::text* value_text = ctx->graphics_menu_value_texts[i];
		if (value_text)
		{
			value_text->set_material(&ctx->menu_font_material);
			value_text->set_font(&ctx->menu_font);
		}
	}
	
	// Align texts
	align_texts(ctx);
	
	// Construct graphics menu callbacks
	auto menu_back_callback = [ctx]()
	{
		application::state next_state;
		next_state.name = "options_menu";
		next_state.enter = std::bind(game::state::options_menu::enter, ctx);
		next_state.exit = std::bind(game::state::options_menu::exit, ctx);
		ctx->app->change_state(next_state);
	};
	
	ctx->controls["menu_down"]->set_activated_callback
	(
		[ctx]()
		{
			++ctx->graphics_menu_index;
			if (ctx->graphics_menu_index >= ctx->graphics_menu_label_texts.size())
				ctx->graphics_menu_index = 0;
			
			update_text_color(ctx);
		}
	);
	ctx->controls["menu_up"]->set_activated_callback
	(
		[ctx]()
		{
			--ctx->graphics_menu_index;
			if (ctx->graphics_menu_index < 0)
				ctx->graphics_menu_index = ctx->graphics_menu_label_texts.size() - 1;
			
			update_text_color(ctx);
		}
	);
	ctx->controls["menu_left"]->set_activated_callback
	(
		[ctx]()
		{
			if (ctx->graphics_menu_left_callbacks[ctx->graphics_menu_index])
				ctx->graphics_menu_left_callbacks[ctx->graphics_menu_index]();
		}
	);
	ctx->controls["menu_right"]->set_activated_callback
	(
		[ctx]()
		{
			if (ctx->graphics_menu_right_callbacks[ctx->graphics_menu_index])
				ctx->graphics_menu_right_callbacks[ctx->graphics_menu_index]();
		}
	);
	ctx->controls["menu_select"]->set_activated_callback
	(
		[ctx]()
		{
			if (ctx->graphics_menu_select_callbacks[ctx->graphics_menu_index])
				ctx->graphics_menu_select_callbacks[ctx->graphics_menu_index]();
		}
	);
	ctx->controls["menu_back"]->set_activated_callback(menu_back_callback);
	
	auto toggle_fullscreen_callback = [ctx]()
	{
		bool fullscreen = !ctx->app->is_fullscreen();
		
		ctx->app->set_fullscreen(fullscreen);
		
		if (!fullscreen)
		{
			int2 resolution;
			resolution.x = (*ctx->config)["windowed_resolution"][0].get<int>();
			resolution.y = (*ctx->config)["windowed_resolution"][1].get<int>();
			
			ctx->app->resize_window(resolution.x, resolution.y);
		}
		
		const std::string string_fullscreen = (*ctx->strings)["graphics_menu_display_mode_fullscreen"];
		const std::string string_window = (*ctx->strings)["graphics_menu_display_mode_window"];;
		ctx->graphics_menu_display_mode_value_text->set_content((fullscreen) ? string_fullscreen : string_window);
		
		// Save display mode config
		(*ctx->config)["fullscreen"] = fullscreen;
	};
	
	auto increase_render_resolution_callback = [ctx]()
	{
		// Increase resolution
		if (ctx->controls["menu_modifier"]->is_active())
			ctx->render_resolution_scale += 0.01f;
		else
			ctx->render_resolution_scale += 0.1f;
		
		// Limit resolution
		if (ctx->render_resolution_scale > 2.0f)
			ctx->render_resolution_scale = 2.0f;
		
		// Update value text
		ctx->graphics_menu_render_resolution_value_text->set_content(std::to_string(static_cast<int>(std::round(ctx->render_resolution_scale * 100.0f))) + "%");
		
		// Update config
		(*ctx->config)["render_resolution"] = ctx->render_resolution_scale;
	};
	
	auto decrease_render_resolution_callback = [ctx]()
	{
		// Increase resolution
		if (ctx->controls["menu_modifier"]->is_active())
			ctx->render_resolution_scale -= 0.01f;
		else
			ctx->render_resolution_scale -= 0.1f;
		
		// Limit resolution
		if (ctx->render_resolution_scale < 0.1f)
			ctx->render_resolution_scale = 0.1f;
		
		// Update value text
		ctx->graphics_menu_render_resolution_value_text->set_content(std::to_string(static_cast<int>(std::round(ctx->render_resolution_scale * 100.0f))) + "%");
		
		// Update config
		(*ctx->config)["render_resolution"] = ctx->render_resolution_scale;
	};
	
	auto increase_font_size_callback = [ctx]()
	{
		// Increase font size
		if (ctx->controls["menu_modifier"]->is_active())
			ctx->font_size += 0.01f;
		else
			ctx->font_size += 0.1f;
		
		// Limit font size
		if (ctx->font_size > 2.0f)
			ctx->font_size = 2.0f;
		
		// Update value text
		ctx->graphics_menu_font_size_value_text->set_content(std::to_string(static_cast<int>(std::round(ctx->font_size * 100.0f))) + "%");
		
		// Update config
		(*ctx->config)["font_size"] = ctx->font_size;
		
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
		
		// Refresh text
		refresh_texts(ctx);
		
		// Realign texts
		align_texts(ctx);
		update_text_tweens(ctx);
	};
	
	auto decrease_font_size_callback = [ctx]()
	{
		// Decrease font size
		if (ctx->controls["menu_modifier"]->is_active())
			ctx->font_size -= 0.01f;
		else
			ctx->font_size -= 0.1f;
		
		// Limit font size
		if (ctx->font_size < 0.1f)
			ctx->font_size = 0.1f;
		
		// Update value text
		ctx->graphics_menu_font_size_value_text->set_content(std::to_string(static_cast<int>(std::round(ctx->font_size * 100.0f))) + "%");
		
		// Update config
		(*ctx->config)["font_size"] = ctx->font_size;
		
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
		
		// Refresh text
		refresh_texts(ctx);
		
		// Realign texts
		align_texts(ctx);
		update_text_tweens(ctx);
	};
	
	auto toggle_vsync_callback = [ctx]()
	{
		bool vsync = !ctx->app->get_vsync();
		
		ctx->app->set_vsync(vsync);
		
		std::string string_on = (*ctx->strings)["on"];
		std::string string_off = (*ctx->strings)["off"];
		ctx->graphics_menu_v_sync_value_text->set_content((vsync) ? string_on : string_off);
		
		// Save v-sync config
		(*ctx->config)["vsync"] = vsync;
	};
	
	auto toggle_dyslexia_font_callback = [ctx]()
	{
		ctx->dyslexia_font = !ctx->dyslexia_font;
		
		std::string string_on = (*ctx->strings)["on"];
		std::string string_off = (*ctx->strings)["off"];
		ctx->graphics_menu_dyslexia_font_value_text->set_content((ctx->dyslexia_font) ? string_on : string_off);
		
		// Save dyslexia font config
		(*ctx->config)["dyslexia_font"] = ctx->dyslexia_font;
		
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
		
		// Refresh text
		refresh_texts(ctx);
		
		// Realign texts
		align_texts(ctx);
		update_text_tweens(ctx);
	};
	
	// Build list of graphics menu callbacks
	ctx->graphics_menu_select_callbacks.push_back(toggle_fullscreen_callback);
	ctx->graphics_menu_select_callbacks.push_back(increase_render_resolution_callback);
	ctx->graphics_menu_select_callbacks.push_back(toggle_vsync_callback);
	ctx->graphics_menu_select_callbacks.push_back(increase_font_size_callback);
	ctx->graphics_menu_select_callbacks.push_back(toggle_dyslexia_font_callback);
	ctx->graphics_menu_select_callbacks.push_back(menu_back_callback);
	
	ctx->graphics_menu_left_callbacks.push_back(toggle_fullscreen_callback);
	ctx->graphics_menu_left_callbacks.push_back(decrease_render_resolution_callback);
	ctx->graphics_menu_left_callbacks.push_back(toggle_vsync_callback);
	ctx->graphics_menu_left_callbacks.push_back(decrease_font_size_callback);
	ctx->graphics_menu_left_callbacks.push_back(toggle_dyslexia_font_callback);
	ctx->graphics_menu_left_callbacks.push_back(nullptr);
	
	ctx->graphics_menu_right_callbacks.push_back(toggle_fullscreen_callback);
	ctx->graphics_menu_right_callbacks.push_back(increase_render_resolution_callback);
	ctx->graphics_menu_right_callbacks.push_back(toggle_vsync_callback);
	ctx->graphics_menu_right_callbacks.push_back(increase_font_size_callback);
	ctx->graphics_menu_right_callbacks.push_back(toggle_dyslexia_font_callback);
	ctx->graphics_menu_right_callbacks.push_back(nullptr);
	
	// Add text objects to UI
	for (std::size_t i = 0; i < ctx->graphics_menu_label_texts.size(); ++i)
	{
		scene::text* label_text = ctx->graphics_menu_label_texts[i];
		scene::text* value_text = ctx->graphics_menu_value_texts[i];
		
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
	
	// Clear menu callbacks
	ctx->graphics_menu_select_callbacks.clear();
	ctx->graphics_menu_left_callbacks.clear();
	ctx->graphics_menu_right_callbacks.clear();
	
	// Destruct graphics menu texts
	for (std::size_t i = 0; i < ctx->graphics_menu_label_texts.size(); ++i)
	{
		scene::text* label_text = ctx->graphics_menu_label_texts[i];
		ctx->ui_scene->remove_object(label_text);
		delete label_text;
		
		scene::text* value_text = ctx->graphics_menu_value_texts[i];
		if (value_text)
		{
			ctx->ui_scene->remove_object(value_text);
			delete value_text;
		}
	}
	ctx->graphics_menu_label_texts.clear();
	ctx->graphics_menu_value_texts.clear();
	
	// Update volumes in config
	//(*ctx->config)["master_volume"] = ctx->master_volume;
	//(*ctx->config)["ambience_volume"] = ctx->ambience_volume;
	//(*ctx->config)["effects_volume"] = ctx->effects_volume;
	
	ctx->ui_clear_pass->set_cleared_buffers(false, true, false);
}

} // namespace graphics_menu
} // namespace state
} // namespace game

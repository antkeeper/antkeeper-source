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

#include "game/states/main-menu.hpp"
#include "game/states/forage.hpp"
#include "game/states/nuptial-flight.hpp"
#include "render/passes/clear-pass.hpp"
#include "resources/resource-manager.hpp"
#include "render/model.hpp"
#include "animation/screen-transition.hpp"
#include "animation/ease.hpp"
#include "animation/timeline.hpp"
#include "application.hpp"
#include <limits>

namespace game {
namespace state {
namespace main_menu {

void enter(game::context* ctx)
{
	ctx->ui_clear_pass->set_cleared_buffers(true, true, false);
	
	// Construct main menu texts
	ctx->main_menu_start_text = new scene::text();
	ctx->main_menu_options_text = new scene::text();
	ctx->main_menu_credits_text = new scene::text();
	ctx->main_menu_quit_text = new scene::text();
	
	// Set content of texts
	ctx->main_menu_start_text->set_content((*ctx->strings)["main_menu_start"]);
	ctx->main_menu_options_text->set_content((*ctx->strings)["main_menu_options"]);
	ctx->main_menu_credits_text->set_content((*ctx->strings)["main_menu_credits"]);
	ctx->main_menu_quit_text->set_content((*ctx->strings)["main_menu_quit"]);
	
	std::vector<scene::text*> texts;
	texts.push_back(ctx->main_menu_start_text);
	texts.push_back(ctx->main_menu_options_text);
	texts.push_back(ctx->main_menu_credits_text);
	texts.push_back(ctx->main_menu_quit_text);
	
	float offset_y = 0.0f;
	
	for (scene::text* text: texts)
	{
		text->set_material(&ctx->menu_font_material);
		text->set_font(&ctx->menu_font);
		text->set_color({1.0f, 1.0f, 1.0f, 0.5f});
		ctx->ui_scene->add_object(text);
		
		// Align text
		const auto& text_aabb = static_cast<const geom::aabb<float>&>(text->get_local_bounds());
		float title_w = text_aabb.max_point.x - text_aabb.min_point.x;
		float title_h = text_aabb.max_point.y - text_aabb.min_point.y;
		text->set_translation({std::round(-title_w * 0.5f), std::round(-title_h * 0.5f) + offset_y, 0.0f});
		
		offset_y -= ctx->menu_font.get_font_metrics().linespace * 1.5f;
		
		// Update menu AABB
	}
	
	// Load pointer
	ctx->ui_pointer = new scene::model_instance();
	ctx->ui_pointer->set_model(ctx->resource_manager->load<render::model>("pointer.mdl"));
	ctx->ui_scene->add_object(ctx->ui_pointer);
	
	// Scale and position pointer
	float pointer_scale = (ctx->menu_font.get_font_metrics().ascent - ctx->menu_font.get_font_metrics().descent) * (1.0f/3.0f);
	ctx->ui_pointer->set_scale({pointer_scale, pointer_scale, pointer_scale});
	
	float advance_x = ctx->menu_font.get_glyph_metrics(U' ').horizontal_advance * 2.0f;
	
	const auto& text_aabb = static_cast<const geom::aabb<float>&>(ctx->main_menu_start_text->get_local_bounds());
	const auto& text_translation = ctx->main_menu_start_text->get_translation();
	
	ctx->ui_pointer->set_translation(text_translation + float3{-advance_x, (text_aabb.max_point.y - text_aabb.min_point.y) * 0.5f, 0.0f});
	ctx->main_menu_start_text->set_color({1.0f, 1.0f, 1.0f, 1.0f});
	
	ctx->controls["menu_down"]->set_activated_callback
	(
		[ctx]()
		{
			ctx->ui_pointer->set_translation(ctx->ui_pointer->get_translation() - float3{0.0f, ctx->menu_font.get_font_metrics().linespace * 1.5f, 0.0f});
		}
	);
	ctx->controls["menu_up"]->set_activated_callback
	(
		[ctx]()
		{
			ctx->ui_pointer->set_translation(ctx->ui_pointer->get_translation() + float3{0.0f, ctx->menu_font.get_font_metrics().linespace * 1.5f, 0.0f});
		}
	);
	ctx->controls["menu_select"]->set_activated_callback
	(
		[ctx]()
		{
			// Disable menu controls
			ctx->controls["menu_down"]->set_activated_callback(nullptr);
			ctx->controls["menu_up"]->set_activated_callback(nullptr);
			ctx->controls["menu_select"]->set_activated_callback(nullptr);
			
			// Create change state functions
			auto change_state_nuptial_flight = [ctx]()
			{
				application::state next_state;
				next_state.name = "nuptial_flight";
				next_state.enter = std::bind(game::state::nuptial_flight::enter, ctx);
				next_state.exit = std::bind(game::state::nuptial_flight::exit, ctx);
				ctx->app->change_state(next_state);
			};
			
			// Set up timing
			const float fade_out_duration = 1.0f;
			
			// Schedule state change
			timeline* timeline = ctx->timeline;
			float t = timeline->get_position();
			timeline::sequence sequence =
			{
				{t + fade_out_duration, change_state_nuptial_flight}
			};
			timeline->add_sequence(sequence);
			
			// Start fade out to white
			ctx->fade_transition_color->set_value({1, 1, 1});
			ctx->fade_transition->transition(fade_out_duration, false, ease<float>::out_quad, false);
		}
	);
	
	// Disable control callbacks
	ctx->controls["menu_down"]->set_callbacks_enabled(false);
	ctx->controls["menu_up"]->set_callbacks_enabled(false);
	ctx->controls["menu_select"]->set_callbacks_enabled(false);
	
	// Start fade in from black
	const float fade_in_duration = 0.5f;
	ctx->fade_transition_color->set_value({0, 0, 0});
	ctx->fade_transition->transition(fade_in_duration, true, ease<float>::in_quad);
	
	// Schedule enabling of control callbacks
	auto enable_control_callbacks = [ctx]()
	{
		ctx->controls["menu_down"]->set_callbacks_enabled(true);
		ctx->controls["menu_up"]->set_callbacks_enabled(true);
		ctx->controls["menu_select"]->set_callbacks_enabled(true);
	};
	timeline* timeline = ctx->timeline;
	float t = timeline->get_position();
	timeline::sequence sequence =
	{
		{t + fade_in_duration, enable_control_callbacks}
	};
	timeline->add_sequence(sequence);
}

void exit(game::context* ctx)
{
	// Remove control callbacks
	ctx->controls["menu_down"]->set_activated_callback(nullptr);
	ctx->controls["menu_up"]->set_activated_callback(nullptr);
	ctx->controls["menu_select"]->set_activated_callback(nullptr);
	
	// Remove text objects from UI
	std::vector<scene::text*> texts;
	texts.push_back(ctx->main_menu_start_text);
	texts.push_back(ctx->main_menu_options_text);
	texts.push_back(ctx->main_menu_credits_text);
	texts.push_back(ctx->main_menu_quit_text);
	for (scene::text* text: texts)
	{
		ctx->ui_scene->remove_object(text);
		delete text;
		text = nullptr;
	}
	
	// Remove pointer from UI
	ctx->ui_scene->remove_object(ctx->ui_pointer);
	delete ctx->ui_pointer;
	ctx->ui_pointer = nullptr;
	
	ctx->ui_clear_pass->set_cleared_buffers(false, true, false);
}

} // namespace main_menu
} // namespace state
} // namespace game

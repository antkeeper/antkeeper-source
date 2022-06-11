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
#include "game/states/title.hpp"
#include "game/states/options-menu.hpp"
#include "game/states/forage.hpp"
#include "game/states/nuptial-flight.hpp"
#include "game/states/credits.hpp"
#include "render/passes/clear-pass.hpp"
#include "resources/resource-manager.hpp"
#include "render/model.hpp"
#include "animation/animation.hpp"
#include "animation/animator.hpp"
#include "animation/screen-transition.hpp"
#include "animation/ease.hpp"
#include "animation/timeline.hpp"
#include "application.hpp"
#include <limits>

namespace game {
namespace state {
namespace main_menu {

void enter(game::context* ctx, int main_menu_index)
{
	ctx->main_menu_index = main_menu_index;
	
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
	
	float menu_height = texts.size() * ctx->menu_font.get_font_metrics().linespace;
	float menu_y = menu_height * 0.5f - ctx->menu_font.get_font_metrics().linespace * 0.5f;
	
	for (std::size_t i = 0; i < texts.size(); ++i)
	{
		scene::text* text = texts[i];
		
		text->set_material(&ctx->menu_font_material);
		text->set_font(&ctx->menu_font);
		text->set_color({1.0f, 1.0f, 1.0f, 0.5f});
		ctx->ui_scene->add_object(text);
		
		// Align text
		const auto& bounds = static_cast<const geom::aabb<float>&>(text->get_local_bounds());
		float w = bounds.max_point.x - bounds.min_point.x;
		float x = -w * 0.5f;
		float y = menu_y - ctx->menu_font.get_font_metrics().linespace * i;
		
		text->set_translation({std::round(x), std::round(y), 0.0f});
	}
	
	float advance_x = ctx->menu_font.get_glyph_metrics(U' ').horizontal_advance * 2.0f;
	
	const auto& text_aabb = static_cast<const geom::aabb<float>&>(ctx->main_menu_start_text->get_local_bounds());
	const auto& text_translation = ctx->main_menu_start_text->get_translation();
	
	ctx->main_menu_start_text->set_color({1.0f, 1.0f, 1.0f, 1.0f});
	
	ctx->controls["menu_down"]->set_activated_callback
	(
		[ctx]()
		{
			++ctx->main_menu_index;
			if (ctx->main_menu_index > 3)
				ctx->main_menu_index = 0;
			
			float4 active_color{1.0f, 1.0f, 1.0f, 1.0f};
			float4 inactive_color{1.0f, 1.0f, 1.0f, 0.5f};
			
			ctx->main_menu_start_text->set_color((ctx->main_menu_index == 0) ? active_color : inactive_color);
			ctx->main_menu_options_text->set_color((ctx->main_menu_index == 1) ? active_color : inactive_color);
			ctx->main_menu_credits_text->set_color((ctx->main_menu_index == 2) ? active_color : inactive_color);
			ctx->main_menu_quit_text->set_color((ctx->main_menu_index == 3) ? active_color : inactive_color);
		}
	);
	ctx->controls["menu_up"]->set_activated_callback
	(
		[ctx]()
		{
			--ctx->main_menu_index;
			if (ctx->main_menu_index < 0)
				ctx->main_menu_index = 3;
			
			float4 active_color{1.0f, 1.0f, 1.0f, 1.0f};
			float4 inactive_color{1.0f, 1.0f, 1.0f, 0.5f};
			
			ctx->main_menu_start_text->set_color((ctx->main_menu_index == 0) ? active_color : inactive_color);
			ctx->main_menu_options_text->set_color((ctx->main_menu_index == 1) ? active_color : inactive_color);
			ctx->main_menu_credits_text->set_color((ctx->main_menu_index == 2) ? active_color : inactive_color);
			ctx->main_menu_quit_text->set_color((ctx->main_menu_index == 3) ? active_color : inactive_color);
		}
	);
	ctx->controls["menu_back"]->set_activated_callback
	(
		[ctx]()
		{
			application::state next_state;
			next_state.name = "title";
			next_state.enter = std::bind(game::state::title::enter, ctx);
			next_state.exit = std::bind(game::state::title::exit, ctx);
			ctx->app->change_state(next_state);
		}
	);
	ctx->controls["menu_select"]->set_activated_callback
	(
		[ctx]()
		{
			if (ctx->main_menu_index == 0)
			{
				// Disable menu controls
				ctx->controls["menu_down"]->set_activated_callback(nullptr);
				ctx->controls["menu_up"]->set_activated_callback(nullptr);
				ctx->controls["menu_select"]->set_activated_callback(nullptr);
				
				// Create change state function
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
				timeline->add_sequence({{t + fade_out_duration, change_state_nuptial_flight}});
				
				// Start fade out to white
				ctx->fade_transition_color->set_value({1, 1, 1});
				ctx->fade_transition->transition(fade_out_duration, false, ease<float>::out_quad, false);
			}
			else if (ctx->main_menu_index == 1)
			{
				// Disable menu controls
				ctx->controls["menu_down"]->set_activated_callback(nullptr);
				ctx->controls["menu_up"]->set_activated_callback(nullptr);
				ctx->controls["menu_select"]->set_activated_callback(nullptr);
				
				// Create change state function
				auto change_state_options = [ctx]()
				{
					application::state next_state;
					next_state.name = "options_menu";
					next_state.enter = std::bind(game::state::options_menu::enter, ctx);
					next_state.exit = std::bind(game::state::options_menu::exit, ctx);
					ctx->app->change_state(next_state);
				};
				
				// Set up timing
				const float fade_out_duration = 0.25f;
				
				// Schedule state change
				timeline* timeline = ctx->timeline;
				float t = timeline->get_position();
				timeline->add_sequence({{t + fade_out_duration, change_state_options}});
				
				// Start fade out to black
				ctx->fade_transition_color->set_value({0, 0, 0});
				ctx->fade_transition->transition(fade_out_duration, false, ease<float>::out_quad);
			}
			else if (ctx->main_menu_index == 2)
			{
				// Disable menu controls
				ctx->controls["menu_down"]->set_activated_callback(nullptr);
				ctx->controls["menu_up"]->set_activated_callback(nullptr);
				ctx->controls["menu_select"]->set_activated_callback(nullptr);
				
				// Create change state function
				auto change_state_credits = [ctx]()
				{
					application::state next_state;
					next_state.name = "credits";
					next_state.enter = std::bind(game::state::credits::enter, ctx);
					next_state.exit = std::bind(game::state::credits::exit, ctx);
					ctx->app->change_state(next_state);
				};
				
				// Set up timing
				const float fade_out_duration = 0.5f;
				
				// Schedule state change
				timeline* timeline = ctx->timeline;
				float t = timeline->get_position();
				timeline->add_sequence({{t + fade_out_duration, change_state_credits}});
				
				// Start fade out to black
				ctx->fade_transition_color->set_value({0, 0, 0});
				ctx->fade_transition->transition(fade_out_duration, false, ease<float>::out_quad);
			}
			else if (ctx->main_menu_index == 3)
			{
				ctx->app->close(EXIT_SUCCESS);
			}
		}
	);
	
	// Disable control callbacks
	ctx->controls["menu_down"]->set_callbacks_enabled(false);
	ctx->controls["menu_up"]->set_callbacks_enabled(false);
	ctx->controls["menu_select"]->set_callbacks_enabled(false);
	
	// Build main menu fade in animation
	ctx->main_menu_fade_animation = new animation<float>();
	animation_channel<float>* main_menu_opacity_channel = ctx->main_menu_fade_animation->add_channel(0);
	ctx->main_menu_fade_animation->set_interpolator(ease<float>::out_quad);
	double main_menu_fade_in_duration = 0.0;
	if (ctx->config->contains("main_menu_fade_in_duration"))
		main_menu_fade_in_duration = (*ctx->config)["main_menu_fade_in_duration"].get<double>();
	main_menu_opacity_channel->insert_keyframe({0.0, 0.0f});
	main_menu_opacity_channel->insert_keyframe({main_menu_fade_in_duration, 1.0f});
	
	// Adjust main menu text opacity
	ctx->main_menu_fade_animation->set_frame_callback
	(
		[ctx](int channel, const float& opacity)
		{
			float4 active_color{1.0f, 1.0f, 1.0f, opacity};
			float4 inactive_color{1.0f, 1.0f, 1.0f, 0.5f * opacity};
			
			ctx->main_menu_start_text->set_color((ctx->main_menu_index == 0) ? active_color : inactive_color);
			ctx->main_menu_options_text->set_color((ctx->main_menu_index == 1) ? active_color : inactive_color);
			ctx->main_menu_credits_text->set_color((ctx->main_menu_index == 2) ? active_color : inactive_color);
			ctx->main_menu_quit_text->set_color((ctx->main_menu_index == 3) ? active_color : inactive_color);
			
			// Enable menu controls when visible
			if (opacity > 0.0f)
			{
				ctx->controls["menu_down"]->set_callbacks_enabled(true);
				ctx->controls["menu_up"]->set_callbacks_enabled(true);
				ctx->controls["menu_select"]->set_callbacks_enabled(true);
			}
		}
	);
	
	ctx->animator->add_animation(ctx->main_menu_fade_animation);
	ctx->main_menu_fade_animation->play();
}

void exit(game::context* ctx)
{
	// Disable menu control callbacks
	ctx->controls["menu_down"]->set_activated_callback(nullptr);
	ctx->controls["menu_up"]->set_activated_callback(nullptr);
	ctx->controls["menu_back"]->set_activated_callback(nullptr);
	ctx->controls["menu_select"]->set_activated_callback(nullptr);
	
	// Destruct main menu animation
	ctx->animator->remove_animation(ctx->main_menu_fade_animation);
	delete ctx->main_menu_fade_animation;
	ctx->main_menu_fade_animation = nullptr;
	
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
	
	ctx->ui_clear_pass->set_cleared_buffers(false, true, false);
}

} // namespace main_menu
} // namespace state
} // namespace game

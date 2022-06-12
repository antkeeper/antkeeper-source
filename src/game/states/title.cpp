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

#include "game/states/title.hpp"
#include "game/states/main-menu.hpp"
#include "animation/screen-transition.hpp"
#include "animation/ease.hpp"
#include "animation/animation.hpp"
#include "animation/animator.hpp"
#include "application.hpp"
#include "scene/text.hpp"
#include "configuration.hpp"
#include "render/passes/clear-pass.hpp"

namespace game {
namespace state {
namespace title {

void enter(game::context* ctx)
{
	ctx->ui_clear_pass->set_cleared_buffers(true, true, false);
	
	// Construct title text
	ctx->title_text = new scene::text();
	ctx->title_text->set_material(&ctx->title_font_material);
	ctx->title_text->set_font(&ctx->title_font);
	ctx->title_text->set_color({1.0f, 1.0f, 1.0f, 0.0f});
	ctx->title_text->set_content((*ctx->strings)["title_antkeeper"]);
	
	// Construct "Press any key" text
	ctx->title_press_any_key_text = new scene::text();
	ctx->title_press_any_key_text->set_material(&ctx->menu_font_material);
	ctx->title_press_any_key_text->set_font(&ctx->menu_font);
	ctx->title_press_any_key_text->set_color({1.0f, 1.0f, 1.0f, 0.0f});
	ctx->title_press_any_key_text->set_content((*ctx->strings)["title_press_any_key"]);
	
	int window_height = std::get<1>(ctx->app->get_viewport_dimensions());
	
	// Align title text
	const auto& title_aabb = static_cast<const geom::aabb<float>&>(ctx->title_text->get_local_bounds());
	float title_w = title_aabb.max_point.x - title_aabb.min_point.x;
	float title_h = title_aabb.max_point.y - title_aabb.min_point.y;
	ctx->title_text->set_translation({std::round(-title_w * 0.5f), std::round(-title_h * 0.5f + (window_height / 3) / 2), 0.0f});
	
	// Align "Press any key" text
	const auto& any_key_aabb = static_cast<const geom::aabb<float>&>(ctx->title_press_any_key_text->get_local_bounds());
	float any_key_w = any_key_aabb.max_point.x - any_key_aabb.min_point.x;
	float any_key_h = any_key_aabb.max_point.y - any_key_aabb.min_point.y;
	ctx->title_press_any_key_text->set_translation({std::round(-any_key_w * 0.5f), std::round(-any_key_h * 0.5f - (window_height / 3) / 2), 0.0f});
	
	// Load animation timing configuration
	double title_fade_in_duration = 0.0;
	double title_fade_out_duration = 0.0;
	double title_press_any_key_duration = 0.0;
	double title_press_any_key_delay = 0.0;
	if (ctx->config->contains("title_fade_in_duration"))
		title_fade_in_duration = (*ctx->config)["title_fade_in_duration"].get<double>();
	if (ctx->config->contains("title_fade_out_duration"))
		title_fade_out_duration = (*ctx->config)["title_fade_out_duration"].get<double>();
	if (ctx->config->contains("title_press_any_key_duration"))
		title_press_any_key_duration = (*ctx->config)["title_press_any_key_duration"].get<double>();
	if (ctx->config->contains("title_press_any_key_delay"))
		title_press_any_key_delay = (*ctx->config)["title_press_any_key_delay"].get<double>();
	
	auto set_title_opacity = [ctx](int channel, const float& opacity)
	{
		ctx->title_text->set_color({1.0f, 1.0f, 1.0f, opacity});
	};
	
	// Build title fade in animation
	ctx->title_fade_in_animation = new animation<float>();
	animation_channel<float>* title_fade_in_opacity_channel = ctx->title_fade_in_animation->add_channel(0);
	ctx->title_fade_in_animation->set_interpolator(ease<float>::in_quad);
	title_fade_in_opacity_channel->insert_keyframe({0.0, 0.0f});
	title_fade_in_opacity_channel->insert_keyframe({title_fade_in_duration, 1.0f});
	title_fade_in_opacity_channel->insert_keyframe({title_fade_in_duration + title_press_any_key_delay, 1.0f});
	ctx->title_fade_in_animation->set_frame_callback(set_title_opacity);
	
	// Trigger "Press any key" animation after title fade in animation ends
	ctx->title_fade_in_animation->set_end_callback
	(
		[ctx]()
		{
			ctx->title_press_any_key_animation->play();
		}
	);
	
	// Build title fade out animation
	ctx->title_fade_out_animation = new animation<float>();
	animation_channel<float>* title_fade_out_opacity_channel = ctx->title_fade_out_animation->add_channel(0);
	ctx->title_fade_out_animation->set_interpolator(ease<float>::out_quad);
	title_fade_out_opacity_channel->insert_keyframe({0.0, 1.0f});
	title_fade_out_opacity_channel->insert_keyframe({title_fade_out_duration, 0.0f});
	ctx->title_fade_out_animation->set_frame_callback(set_title_opacity);
	
	// Trigger a state change when the title fade out animation ends
	ctx->title_fade_out_animation->set_end_callback
	(
		[ctx]()
		{
			application::state next_state;
			next_state.name = "main_menu";
			next_state.enter = std::bind(game::state::main_menu::enter, ctx);
			next_state.exit = std::bind(game::state::main_menu::exit, ctx);
			ctx->app->queue_state(next_state);
		}
	);
	
	// Build "Press any key" animation
	ctx->title_press_any_key_animation = new animation<float>();
	ctx->title_press_any_key_animation->loop(true);
	animation_channel<float>* title_press_any_key_opacity_channel = ctx->title_press_any_key_animation->add_channel(0);
	ctx->title_press_any_key_animation->set_interpolator(math::lerp<float, double>);
	title_press_any_key_opacity_channel->insert_keyframe({0.0, 0.0f});
	title_press_any_key_opacity_channel->insert_keyframe({title_press_any_key_duration * 0.5, 1.0f});
	title_press_any_key_opacity_channel->insert_keyframe({title_press_any_key_duration, 0.0f});
	ctx->title_press_any_key_animation->set_frame_callback
	(
		[ctx](int channel, const float& opacity)
		{
			ctx->title_press_any_key_text->set_color({1.0f, 1.0f, 1.0f, 0.5f * ease<float>::out_cubic(0.0f, 1.0f, opacity)});
		}
	);
	
	// Add title fade animations to animator
	ctx->animator->add_animation(ctx->title_fade_in_animation);
	ctx->animator->add_animation(ctx->title_fade_out_animation);
	ctx->animator->add_animation(ctx->title_press_any_key_animation);
	
	// Start title fade in animation
	ctx->title_fade_in_animation->play();
	
	// Set up title skipper
	ctx->input_listener->set_callback
	(
		[ctx](const event_base& event)
		{
			if (ctx->controls["menu_back"]->is_active())
				return;
			
			auto id = event.get_event_type_id();
			if (id != mouse_moved_event::event_type_id && id != mouse_wheel_scrolled_event::event_type_id && id != gamepad_axis_moved_event::event_type_id)
			{
				/*
				if (ctx->title_fade_in_animation->is_stopped())
				{
					ctx->title_fade_out_animation->play();	
					ctx->input_listener->set_enabled(false);
				}
				*/
				
				if (ctx->title_text->get_color()[3] > 0.0f)
				{
					ctx->input_listener->set_enabled(false);
					
					// Black out screen
					ctx->rasterizer->set_clear_color(0.0f, 0.0f, 0.0f, 1.0f);
					ctx->rasterizer->clear_framebuffer(true, false, false);
					ctx->app->swap_buffers();
					
					// Change state
					application::state next_state;
					next_state.name = "main_menu";
					next_state.enter = std::bind(game::state::main_menu::enter, ctx);
					next_state.exit = std::bind(game::state::main_menu::exit, ctx);
					ctx->app->change_state(next_state);
				}
			}
		}
	);
	ctx->input_listener->set_enabled(true);
	
	ctx->ui_scene->add_object(ctx->title_text);
	ctx->title_text->update_tweens();
	
	ctx->ui_scene->add_object(ctx->title_press_any_key_text);
	ctx->title_press_any_key_text->update_tweens();
}

void exit(game::context* ctx)
{
	// Remove title text
	ctx->ui_scene->remove_object(ctx->title_text);
	ctx->ui_scene->remove_object(ctx->title_press_any_key_text);
	
	// Disable title skipper
	ctx->input_listener->set_enabled(false);
	ctx->input_listener->set_callback(nullptr);
	
	// Destruct title animations
	ctx->animator->remove_animation(ctx->title_fade_in_animation);
	ctx->animator->remove_animation(ctx->title_fade_out_animation);
	ctx->animator->remove_animation(ctx->title_press_any_key_animation);
	
	delete ctx->title_fade_in_animation;
	delete ctx->title_fade_out_animation;
	delete ctx->title_press_any_key_animation;
	ctx->title_fade_in_animation = nullptr;
	ctx->title_fade_out_animation = nullptr;
	ctx->title_press_any_key_animation = nullptr;
	
	ctx->ui_clear_pass->set_cleared_buffers(false, true, false);
}

} // namespace title
} // namespace state
} // namespace game

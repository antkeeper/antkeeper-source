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

#include "game/states/splash.hpp"
#include "game/states/main-menu.hpp"
#include "animation/screen-transition.hpp"
#include "animation/animation.hpp"
#include "animation/animator.hpp"
#include "animation/ease.hpp"
#include "animation/timeline.hpp"
#include "application.hpp"
#include "render/passes/clear-pass.hpp"

namespace game {
namespace state {
namespace splash {

void enter(game::context* ctx)
{
	ctx->ui_clear_pass->set_cleared_buffers(true, true, false);
	
	// Load animation timing configuration
	double splash_fade_in_duration = 0.0;
	double splash_duration = 0.0;
	double splash_fade_out_duration = 0.0;
	if (ctx->config->contains("splash_fade_in_duration"))
		splash_fade_in_duration = (*ctx->config)["splash_fade_in_duration"].get<double>();
	if (ctx->config->contains("splash_duration"))
		splash_duration = (*ctx->config)["splash_duration"].get<double>();
	if (ctx->config->contains("splash_fade_out_duration"))
		splash_fade_out_duration = (*ctx->config)["splash_fade_out_duration"].get<double>();
	
	// Build splash fade in animation
	ctx->splash_fade_in_animation = new animation<float>();
	animation_channel<float>* splash_fade_in_opacity_channel = ctx->splash_fade_in_animation->add_channel(0);
	ctx->splash_fade_in_animation->set_interpolator(ease<float>::out_cubic);
	splash_fade_in_opacity_channel->insert_keyframe({0.0, 0.0f});
	splash_fade_in_opacity_channel->insert_keyframe({splash_fade_in_duration, 1.0f});
	splash_fade_in_opacity_channel->insert_keyframe({splash_fade_in_duration + splash_duration, 1.0f});
	
	// Build splash fade out animation
	ctx->splash_fade_out_animation = new animation<float>();
	animation_channel<float>* splash_fade_out_opacity_channel = ctx->splash_fade_out_animation->add_channel(0);
	ctx->splash_fade_out_animation->set_interpolator(ease<float>::out_cubic);
	splash_fade_out_opacity_channel->insert_keyframe({0.0, 1.0f});
	splash_fade_out_opacity_channel->insert_keyframe({splash_fade_out_duration, 0.0f});
	
	// Setup animation frame callbacks
	auto set_splash_opacity = [ctx](int channel, const float& opacity)
	{
		static_cast<render::material_property<float4>*>(ctx->splash_billboard_material->get_property("tint"))->set_value(float4{1, 1, 1, opacity});
	};
	ctx->splash_fade_in_animation->set_frame_callback(set_splash_opacity);
	ctx->splash_fade_out_animation->set_frame_callback(set_splash_opacity);
	
	// Reset splash color when animation starts
	ctx->splash_fade_in_animation->set_start_callback
	(
		[ctx]()
		{
			static_cast<render::material_property<float4>*>(ctx->splash_billboard_material->get_property("tint"))->set_value(float4{1, 1, 1, 0});
			ctx->splash_billboard_material->update_tweens();
		}
	);
	
	// Trigger splash fade out animation when splash fade in animation ends
	ctx->splash_fade_in_animation->set_end_callback
	(
		[ctx]()
		{
			ctx->splash_fade_out_animation->play();
		}
	);
	
	// Trigger a state change when the splash fade out animation ends
	ctx->splash_fade_out_animation->set_end_callback
	(
		[ctx]()
		{
			application::state next_state;
			next_state.name = "main_menu";
			next_state.enter = std::bind(game::state::main_menu::enter, ctx, true);
			next_state.exit = std::bind(game::state::main_menu::exit, ctx);
			ctx->app->queue_state(next_state);
		}
	);
	
	// Add splash fade animations to animator
	ctx->animator->add_animation(ctx->splash_fade_in_animation);
	ctx->animator->add_animation(ctx->splash_fade_out_animation);
	
	// Start splash fade in animation
	ctx->splash_fade_in_animation->play();
	
	// Set up splash skipper
	ctx->input_listener->set_callback
	(
		[ctx](const event_base& event)
		{
			auto id = event.get_event_type_id();
			if (id != mouse_moved_event::event_type_id && id != mouse_wheel_scrolled_event::event_type_id && id != gamepad_axis_moved_event::event_type_id)
			{
				// Black out screen
				ctx->rasterizer->set_clear_color(0.0f, 0.0f, 0.0f, 1.0f);
				ctx->rasterizer->clear_framebuffer(true, false, false);
				ctx->app->swap_buffers();
				
				// Change state
				application::state next_state;
				next_state.name = "main_menu";
				next_state.enter = std::bind(game::state::main_menu::enter, ctx, true);
				next_state.exit = std::bind(game::state::main_menu::exit, ctx);
				ctx->app->change_state(next_state);
			}
		}
	);
	ctx->input_listener->set_enabled(true);
	
	// Add splash billboard to UI scene
	ctx->ui_scene->add_object(ctx->splash_billboard);
}

void exit(game::context* ctx)
{
	// Remove splash billboard from UI scene
	ctx->ui_scene->remove_object(ctx->splash_billboard);
	
	// Disable splash skipper
	ctx->input_listener->set_enabled(false);
	ctx->input_listener->set_callback(nullptr);
	
	// Destruct splash fade animations
	ctx->animator->remove_animation(ctx->splash_fade_in_animation);
	ctx->animator->remove_animation(ctx->splash_fade_out_animation);
	delete ctx->splash_fade_in_animation;
	delete ctx->splash_fade_out_animation;
	ctx->splash_fade_in_animation = nullptr;
	ctx->splash_fade_out_animation = nullptr;
	
	ctx->ui_clear_pass->set_cleared_buffers(false, true, false);
}

} // namespace splash
} // namespace state
} // namespace game

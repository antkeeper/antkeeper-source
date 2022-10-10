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

#include "game/state/splash.hpp"
#include "game/state/main-menu.hpp"
#include "game/state/nuptial-flight.hpp"
#include "animation/screen-transition.hpp"
#include "animation/animation.hpp"
#include "animation/animator.hpp"
#include "animation/ease.hpp"
#include "application.hpp"
#include "render/passes/clear-pass.hpp"
#include "game/context.hpp"
#include "debug/logger.hpp"
#include "resources/resource-manager.hpp"
#include "render/material-flags.hpp"

namespace game {
namespace state {

splash::splash(game::context& ctx):
	game::state::base(ctx)
{
	ctx.logger->push_task("Entering splash state");
	
	// Enable color buffer clearing in UI pass
	ctx.ui_clear_pass->set_cleared_buffers(true, true, false);
	
	// Load splash texture
	const gl::texture_2d* splash_texture = ctx.resource_manager->load<gl::texture_2d>("splash.tex");
	
	// Get splash texture dimensions
	auto splash_dimensions = splash_texture->get_dimensions();
	
	// Construct splash billboard material
	splash_billboard_material.set_flags(MATERIAL_FLAG_TRANSLUCENT);
	splash_billboard_material.set_shader_program(ctx.resource_manager->load<gl::shader_program>("ui-element-textured.glsl"));
	splash_billboard_material.add_property<const gl::texture_2d*>("background")->set_value(splash_texture);
	render::material_property<float4>* splash_tint = splash_billboard_material.add_property<float4>("tint");
	splash_tint->set_value(float4{1, 1, 1, 0});
	splash_billboard_material.update_tweens();
	
	// Construct splash billboard
	splash_billboard.set_material(&splash_billboard_material);
	splash_billboard.set_scale({(float)std::get<0>(splash_dimensions) * 0.5f, (float)std::get<1>(splash_dimensions) * 0.5f, 1.0f});
	splash_billboard.set_translation({0.0f, 0.0f, 0.0f});
	splash_billboard.update_tweens();
	
	// Add splash billboard to UI scene
	ctx.ui_scene->add_object(&splash_billboard);
	
	// Load animation timing configuration
	double splash_fade_in_duration = 0.0;
	double splash_duration = 0.0;
	double splash_fade_out_duration = 0.0;
	if (ctx.config->contains("splash_fade_in_duration"))
		splash_fade_in_duration = (*ctx.config)["splash_fade_in_duration"].get<double>();
	if (ctx.config->contains("splash_duration"))
		splash_duration = (*ctx.config)["splash_duration"].get<double>();
	if (ctx.config->contains("splash_fade_out_duration"))
		splash_fade_out_duration = (*ctx.config)["splash_fade_out_duration"].get<double>();
	
	// Construct splash fade in animation
	splash_fade_in_animation.set_interpolator(ease<float>::out_cubic);
	animation_channel<float>* splash_fade_in_opacity_channel = splash_fade_in_animation.add_channel(0);
	splash_fade_in_opacity_channel->insert_keyframe({0.0, 0.0f});
	splash_fade_in_opacity_channel->insert_keyframe({splash_fade_in_duration, 1.0f});
	splash_fade_in_opacity_channel->insert_keyframe({splash_fade_in_duration + splash_duration, 1.0f});
	
	// Build splash fade out animation
	splash_fade_out_animation.set_interpolator(ease<float>::out_cubic);
	animation_channel<float>* splash_fade_out_opacity_channel = splash_fade_out_animation.add_channel(0);
	splash_fade_out_opacity_channel->insert_keyframe({0.0, 1.0f});
	splash_fade_out_opacity_channel->insert_keyframe({splash_fade_out_duration, 0.0f});
	
	// Setup animation frame callbacks
	auto set_splash_opacity = [splash_tint](int channel, const float& opacity)
	{
		splash_tint->set_value(float4{1, 1, 1, opacity});
	};
	splash_fade_in_animation.set_frame_callback(set_splash_opacity);
	splash_fade_out_animation.set_frame_callback(set_splash_opacity);
	
	// Trigger splash fade out animation when splash fade in animation ends
	splash_fade_in_animation.set_end_callback
	(
		[this]()
		{
			this->splash_fade_out_animation.play();
		}
	);
	
	// Trigger a state change when the splash fade out animation ends
	splash_fade_out_animation.set_end_callback
	(
		[&ctx]()
		{
			// Queue change to main menu state
			ctx.function_queue.push
			(
				[&ctx]()
				{
					ctx.state_machine.pop();
					ctx.state_machine.emplace(new game::state::main_menu(ctx, true));
					//ctx.state_machine.emplace(new game::state::nuptial_flight(ctx));
				}
			);
		}
	);
	
	// Add splash fade animations to animator
	ctx.animator->add_animation(&splash_fade_in_animation);
	ctx.animator->add_animation(&splash_fade_out_animation);
	
	// Start splash fade in animation
	splash_fade_in_animation.play();
	
	// Set up splash skipper
	ctx.input_listener->set_callback
	(
		[&ctx](const event_base& event)
		{
			auto id = event.get_event_type_id();
			if (id != mouse_moved_event::event_type_id && id != mouse_wheel_scrolled_event::event_type_id && id != gamepad_axis_moved_event::event_type_id)
			{
				// Black out screen
				ctx.rasterizer->set_clear_color(0.0f, 0.0f, 0.0f, 1.0f);
				ctx.rasterizer->clear_framebuffer(true, false, false);
				ctx.app->swap_buffers();
				
				// Change to main menu state
				ctx.state_machine.pop();
				ctx.state_machine.emplace(new game::state::main_menu(ctx, true));
				//ctx.state_machine.emplace(new game::state::nuptial_flight(ctx));
			}
		}
	);
	ctx.input_listener->set_enabled(true);
	
	ctx.logger->pop_task(EXIT_SUCCESS);
}

splash::~splash()
{
	ctx.logger->push_task("Exiting splash state");
	
	// Disable splash skipper
	ctx.input_listener->set_enabled(false);
	ctx.input_listener->set_callback(nullptr);
	
	// Remove splash fade animations from animator
	ctx.animator->remove_animation(&splash_fade_in_animation);
	ctx.animator->remove_animation(&splash_fade_out_animation);
	
	// Remove splash billboard from UI scene
	ctx.ui_scene->remove_object(&splash_billboard);
	
	// Unload splash texture
	ctx.resource_manager->unload("splash.tex");
	
	// Disable color buffer clearing in UI pass
	ctx.ui_clear_pass->set_cleared_buffers(false, true, false);
	
	ctx.logger->pop_task(EXIT_SUCCESS);
}

} // namespace state
} // namespace game

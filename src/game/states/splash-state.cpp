// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include "game/states/splash-state.hpp"
#include "game/game.hpp"
#include "game/states/main-menu-state.hpp"
#include "game/components/animation-component.hpp"
#include <engine/animation/ease.hpp>
#include <engine/debug/log.hpp>
#include <engine/math/vector.hpp>
#include <engine/render/material-flags.hpp>
#include <engine/resources/resource-manager.hpp>
#include <engine/gl/pipeline.hpp>

splash_state::splash_state(::game& ctx):
	game_state(ctx)
{
	debug::log_trace("Entering splash state...");
	
	const math::fvec2 viewport_size = math::fvec2(ctx.window->get_viewport_size());
	const math::fvec2 viewport_center = viewport_size * 0.5f;
	
	// Load splash texture
	auto splash_texture = ctx.resource_manager->load<gl::texture_2d>("splash.tex");
	
	// Get splash texture dimensions
	const auto& splash_dimensions = splash_texture->get_image_view()->get_image()->get_dimensions();
	
	// Construct splash billboard material
	splash_billboard_material = std::make_shared<render::material>();
	splash_billboard_material->set_blend_mode(render::material_blend_mode::translucent);
	splash_billboard_material->set_shader_template(ctx.resource_manager->load<gl::shader_template>("ui-element-textured.glsl"));
	splash_billboard_material->set_variable("background", std::make_shared<render::matvar_texture_2d>(1, splash_texture));
	
	auto splash_tint = std::make_shared<render::matvar_fvec4>(1, math::fvec4{1, 1, 1, 1});
	splash_billboard_material->set_variable("tint", splash_tint);
	
	// Construct splash billboard
	splash_billboard.set_material(splash_billboard_material);
	splash_billboard.set_scale({static_cast<float>(splash_dimensions[0]) * 0.5f, static_cast<float>(splash_dimensions[1]) * 0.5f, 1.0f});
	splash_billboard.set_translation({std::round(viewport_center.x()), std::round(viewport_center.y()), 0.0f});
	
	// Add splash billboard to UI scene
	ctx.ui_canvas->get_scene().add_object(splash_billboard);

	// Setup animation timing configuration
	const auto splash_fade_in_duration = 0.5f;
	const auto splash_duration = 2.0f;
	const auto splash_fade_out_duration = 0.5f;
	
	// Construct splash sequence
	{
		m_splash_sequence = std::make_shared<animation_sequence>();
		auto& opacity_track = m_splash_sequence->tracks()["opacity"];
		auto& opacity_channel = opacity_track.channels().emplace_back();
		opacity_channel.keyframes().emplace(0.0f, 0.0f);
		opacity_channel.keyframes().emplace(splash_fade_in_duration, 1.0f);
		opacity_channel.keyframes().emplace(opacity_channel.duration() + splash_duration, 1.0f);
		opacity_channel.keyframes().emplace(opacity_channel.duration() + splash_fade_out_duration, 0.0f);
		opacity_track.output() = [splash_tint](auto samples, auto&)
		{
			splash_tint->set(math::fvec4{1, 1, 1, samples[0]});
		};
		
		m_splash_sequence->cues().emplace(opacity_track.duration(), [&](auto&)
		{
			// Queue change to main menu state
			ctx.function_queue.push
			(
				[&ctx]()
				{
					ctx.state_machine.pop();
					ctx.state_machine.emplace(std::make_unique<main_menu_state>(ctx, true));
				}
			);
		});
	}
	
	// Constructs credits entity
	m_splash_entity = ctx.entity_registry->create();
	ctx.entity_registry->emplace<animation_component>(m_splash_entity);
	
	// Play splash sequence
	auto& splash_player = ctx.entity_registry->get<animation_component>(m_splash_entity).player;
	splash_player.play(m_splash_sequence);
	
	// Setup window resized callback
	window_resized_subscription = ctx.window->get_resized_channel().subscribe
	(
		[&](const auto& event)
		{
			const math::fvec2 viewport_size = math::fvec2(event.window->get_viewport_size());
			const math::fvec2 viewport_center = viewport_size * 0.5f;
			splash_billboard.set_translation({std::round(viewport_center.x()), std::round(viewport_center.y()), 0.0f});
		}
	);
	
	// Construct splash skip function
	auto skip = [&]([[maybe_unused]] const auto& event)
	{
		ctx.function_queue.emplace
		(
			[&]()
			{
				// Black out screen
				ctx.window->get_graphics_pipeline().clear_attachments(gl::color_clear_bit, {{0.0f, 0.0f, 0.0f, 0.0f}});
				ctx.window->swap_buffers();
				
				// Change to main menu state
				ctx.state_machine.pop();
				ctx.state_machine.emplace(std::make_unique<main_menu_state>(ctx, true));
			}
		);
	};
	
	// Set up splash skippers
	input_mapped_subscriptions.emplace_back
	(
		ctx.input_mapper.get_gamepad_button_mapped_channel().subscribe
		(
			skip
		)
	);
	input_mapped_subscriptions.emplace_back
	(
		ctx.input_mapper.get_key_mapped_channel().subscribe
		(
			skip
		)
	);
	input_mapped_subscriptions.emplace_back
	(
		ctx.input_mapper.get_mouse_button_mapped_channel().subscribe
		(
			skip
		)
	);
	
	// Enable splash skippers next frame
	ctx.function_queue.push
	(
		[&]()
		{
			ctx.input_mapper.connect(ctx.input_manager->get_event_dispatcher());
		}
	);
	
	debug::log_trace("Entered splash state");
}

splash_state::~splash_state()
{
	debug::log_trace("Exiting splash state...");
	
	// Disable splash skippers
	ctx.input_mapper.disconnect();
	input_mapped_subscriptions.clear();
	
	// Destruct splash entity
	ctx.entity_registry->destroy(m_splash_entity);
	
	// Remove splash billboard from UI scene
	ctx.ui_canvas->get_scene().remove_object(splash_billboard);
	
	debug::log_trace("Exited splash state");
}

/*
 * Copyright (C) 2023  Christopher J. Howard
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

#include "game/states/main-menu-state.hpp"
#include <engine/animation/animation.hpp>
#include <engine/animation/animator.hpp>
#include <engine/animation/ease.hpp>
#include <engine/animation/screen-transition.hpp>
#include <engine/config.hpp>
#include "game/components/steering-component.hpp"
#include "game/components/transform-component.hpp"
#include "game/controls.hpp"
#include "game/ecoregion.hpp"
#include "game/menu.hpp"
#include "game/states/collection-menu-state.hpp"
#include "game/states/extras-menu-state.hpp"
#include "game/states/nuptial-flight-state.hpp"
#include "game/states/nest-selection-state.hpp"
#include "game/states/nest-view-state.hpp"
#include "game/states/options-menu-state.hpp"
#include "game/strings.hpp"
#include "game/world.hpp"
#include <engine/math/glsl.hpp>
#include <engine/math/projection.hpp>
#include <engine/physics/light/exposure.hpp>
#include <engine/render/model.hpp>
#include <engine/render/passes/clear-pass.hpp>
#include <engine/render/passes/ground-pass.hpp>
#include <engine/render/passes/sky-pass.hpp>
#include <engine/resources/resource-manager.hpp>
#include <engine/utility/hash/fnv1a.hpp>
#include <format>
#include <limits>

using namespace math::glsl;

main_menu_state::main_menu_state(::game& ctx, bool fade_in):
	game_state(ctx)
{
	debug::log::trace("Entering main menu state...");
	
	ctx.ui_clear_pass->set_cleared_buffers(true, true, false);
	
	const vec2 viewport_size = vec2(ctx.window->get_viewport_size());
	const vec2 viewport_center = viewport_size * 0.5f;
	
	// Construct title text
	title_text = std::make_unique<scene::text>();
	title_text->set_material(ctx.title_font_material);
	title_text->set_color({1.0f, 1.0f, 1.0f, (fade_in) ? 1.0f : 0.0f});
	title_text->set_font(&ctx.title_font);
	title_text->set_content(get_string(ctx, "title_antkeeper"));
	const auto& title_aabb = title_text->get_bounds();
	float title_w = title_aabb.max.x() - title_aabb.min.x();
	float title_h = title_aabb.max.y() - title_aabb.min.y();
	title_text->set_translation({std::round(viewport_center.x() - title_w * 0.5f), std::round(viewport_center.y() - title_h * 0.5f + (viewport_size.y() / 3.0f) / 2.0f), 0.0f});
	
	// Add text to UI
	ctx.ui_scene->add_object(*title_text);
	
	// Construct title fade animation
	title_fade_animation.set_interpolator(ease<float>::out_cubic);
	animation_channel<float>* opacity_channel = title_fade_animation.add_channel(0);
	title_fade_animation.set_frame_callback
	(
		[this, &ctx](int channel, const float& opacity)
		{
			float4 color = this->title_text->get_color();
			color[3] = opacity;
			this->title_text->set_color(color);
		}
	);
	ctx.animator->add_animation(&title_fade_animation);
	
	// Construct menu item texts
	start_text = std::make_unique<scene::text>();
	options_text = std::make_unique<scene::text>();
	extras_text = std::make_unique<scene::text>();
	quit_text = std::make_unique<scene::text>();
	
	// Build list of menu item texts
	ctx.menu_item_texts.push_back({start_text.get(), nullptr});
	ctx.menu_item_texts.push_back({options_text.get(), nullptr});
	ctx.menu_item_texts.push_back({extras_text.get(), nullptr});
	ctx.menu_item_texts.push_back({quit_text.get(), nullptr});
	
	// Set content of menu item texts
	start_text->set_content(get_string(ctx, "main_menu_start"));
	options_text->set_content(get_string(ctx, "main_menu_options"));
	extras_text->set_content(get_string(ctx, "main_menu_extras"));
	quit_text->set_content(get_string(ctx, "main_menu_quit"));
	
	// Init menu item index
	::menu::init_menu_item_index(ctx, "main");
	
	::menu::update_text_color(ctx);
	::menu::update_text_font(ctx);
	::menu::align_text(ctx, true, false, (-viewport_size.y() / 3.0f) / 2.0f);
	::menu::add_text_to_ui(ctx);
	::menu::setup_animations(ctx);
	
	auto select_start_callback = [this, &ctx]()
	{
		// Disable menu controls
		ctx.function_queue.push(std::bind(::disable_menu_controls, std::ref(ctx)));
		
		// Create change state function
		auto change_state = [&ctx]()
		{
			// Queue change to nuptial state
			ctx.function_queue.push
			(
				[&ctx]()
				{
					ctx.state_machine.pop();
					// ctx.state_machine.emplace(std::make_unique<nuptial_flight_state>(ctx));
					// ctx.state_machine.emplace(std::make_unique<collection_menu_state>(ctx));
					// ctx.state_machine.emplace(std::make_unique<nest_selection_state>(ctx));
					ctx.state_machine.emplace(std::make_unique<nest_view_state>(ctx));
				}
			);
		};
		
		// Fade out title
		this->fade_out_title();
		
		// Fade out menu
		::menu::fade_out(ctx, nullptr);
		
		// Start fade out to white
		//ctx.fade_transition_color->set_value({1, 1, 1});
		ctx.fade_transition_color->set({0, 0, 0});
		ctx.fade_transition->transition(config::new_colony_fade_out_duration, false, ease<float>::out_cubic, false, change_state);
	};
	auto select_options_callback = [this, &ctx]()
	{
		// Disable menu controls
		ctx.function_queue.push(std::bind(::disable_menu_controls, std::ref(ctx)));
		
		// Fade out title
		this->fade_out_title();
		
		// Fade out menu
		::menu::fade_out
		(
			ctx,
			[&ctx]()
			{
				// Queue change to options menu state
				ctx.function_queue.push
				(
					[&ctx]()
					{
						ctx.state_machine.pop();
						ctx.state_machine.emplace(std::make_unique<options_menu_state>(ctx));
					}
				);
			}
		);
	};
	auto select_extras_callback = [this, &ctx]()
	{
		// Disable menu controls
		ctx.function_queue.push(std::bind(::disable_menu_controls, std::ref(ctx)));
		
		// Fade out title
		this->fade_out_title();
		
		// Fade out menu
		::menu::fade_out
		(
			ctx,
			[&ctx]()
			{
				// Queue change to extras menu state
				ctx.function_queue.push
				(
					[&ctx]()
					{
						ctx.state_machine.pop();
						ctx.state_machine.emplace(std::make_unique<extras_menu_state>(ctx));
					}
				);
			}
		);
	};
	auto select_quit_callback = [this, &ctx]()
	{
		// Disable menu controls
		ctx.function_queue.push(std::bind(::disable_menu_controls, std::ref(ctx)));
		
		// Fade out title
		this->fade_out_title();
		
		// Fade out menu
		::menu::fade_out(ctx, nullptr);
		
		// Fade to black then quit
		ctx.fade_transition->transition(config::quit_fade_out_duration, false, ease<float>::out_cubic, false, [&ctx](){ctx.closed=true;});
		
		// Quit immediately
		//ctx.function_queue.push([&ctx](){ctx.closed=true;});
	};
	
	// Build list of menu select callbacks
	ctx.menu_select_callbacks.push_back(select_start_callback);
	ctx.menu_select_callbacks.push_back(select_options_callback);
	ctx.menu_select_callbacks.push_back(select_extras_callback);
	ctx.menu_select_callbacks.push_back(select_quit_callback);
	
	// Build list of menu left callbacks
	ctx.menu_left_callbacks.push_back(nullptr);
	ctx.menu_left_callbacks.push_back(nullptr);
	ctx.menu_left_callbacks.push_back(nullptr);
	ctx.menu_left_callbacks.push_back(nullptr);
	
	// Build list of menu right callbacks
	ctx.menu_right_callbacks.push_back(nullptr);
	ctx.menu_right_callbacks.push_back(nullptr);
	ctx.menu_right_callbacks.push_back(nullptr);
	ctx.menu_right_callbacks.push_back(nullptr);
	
	// Set menu back callback
	ctx.menu_back_callback = select_quit_callback;
	
	if (fade_in)
	{
		// Fade in from black
		ctx.fade_transition->transition(config::title_fade_in_duration, true, ease<float>::out_cubic);
	}
	else
	{
		// Fade in text
		fade_in_title();
		::menu::fade_in(ctx, nullptr);
	}
	
	if (ctx.entities.find("earth") == ctx.entities.end())
	{
		::world::cosmogenesis(ctx);
		::world::create_observer(ctx);
		//::world::enter_ecoregion(ctx, *ctx.resource_manager->load<::ecoregion>("seedy-scrub.eco"));
	}
	
	// Set world time
	::world::set_time(ctx, 2022, 6, 21, 12, 0, 0.0);
	
	// Set world time scale
	::world::set_time_scale(ctx, 0.0);
	
	const float ev100_sunny16 = physics::light::ev::from_settings(16.0f, 1.0f / 100.0f, 100.0f);
	ctx.surface_camera->set_exposure_value(ev100_sunny16);
	
	const float aspect_ratio = viewport_size.x() / viewport_size.y();
	float fov = math::vertical_fov(math::radians(100.0f), aspect_ratio);
	
	ctx.surface_camera->look_at({0, 2.0f, 0}, {0, 0, 0}, {0, 0, 1});
	ctx.surface_camera->set_perspective(fov, ctx.surface_camera->get_aspect_ratio(), ctx.surface_camera->get_clip_near(), ctx.surface_camera->get_clip_far());
	
	// Setup and enable sky and ground passes
	ctx.sky_pass->set_enabled(true);
	ctx.ground_pass->set_enabled(true);
	
	// Disable UI color clear
	ctx.ui_clear_pass->set_cleared_buffers(false, true, false);
	
	// Setup window resized callback
	window_resized_subscription = ctx.window->get_resized_channel().subscribe
	(
		[&](const auto& event)
		{
			const vec2 viewport_size = vec2(event.window->get_viewport_size());
			const vec2 viewport_center = viewport_size * 0.5f;
			
			// Re-align title text
			const auto& title_aabb = title_text->get_bounds();
			float title_w = title_aabb.max.x() - title_aabb.min.x();
			float title_h = title_aabb.max.y() - title_aabb.min.y();
			title_text->set_translation({std::round(viewport_center.x() - title_w * 0.5f), std::round(viewport_center.y() - title_h * 0.5f + (viewport_size.y() / 3.0f) / 2.0f), 0.0f});
			
			::menu::align_text(ctx, true, false, (-viewport_size.y() / 3.0f) / 2.0f);
		}
	);
	
	// Enable menu controls
	ctx.function_queue.push(std::bind(::enable_menu_controls, std::ref(ctx)));
	
	debug::log::trace("Entered main menu state");
}

main_menu_state::~main_menu_state()
{
	debug::log::trace("Exiting main menu state...");
	
	// Destruct menu
	::disable_menu_controls(ctx);
	::menu::clear_callbacks(ctx);
	::menu::delete_animations(ctx);
	::menu::remove_text_from_ui(ctx);
	::menu::delete_text(ctx);
	
	// Hide menu BG
	//ctx.menu_bg_billboard->set_active(false);
	
	// Destruct title animation
	ctx.animator->remove_animation(&title_fade_animation);
	
	// Destruct text
	ctx.ui_scene->remove_object(*title_text);
	
	debug::log::trace("Exited main menu state");
}

void main_menu_state::fade_in_title()
{
	animation_channel<float>* opacity_channel = title_fade_animation.get_channel(0);
	opacity_channel->remove_keyframes();
	opacity_channel->insert_keyframe({0.0f, 0.0f});
	opacity_channel->insert_keyframe({config::menu_fade_in_duration, 1.0f});
	title_fade_animation.stop();
	title_fade_animation.play();
}

void main_menu_state::fade_out_title()
{
	animation_channel<float>* opacity_channel = title_fade_animation.get_channel(0);
	opacity_channel->remove_keyframes();
	opacity_channel->insert_keyframe({0.0f, 1.0f});
	opacity_channel->insert_keyframe({config::menu_fade_out_duration, 0.0f});
	title_fade_animation.stop();
	title_fade_animation.play();
}

// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include "game/states/main-menu-state.hpp"
#include <engine/animation/ease.hpp>
#include <engine/config.hpp>
#include "game/components/steering-component.hpp"
#include "game/components/transform-component.hpp"
#include "game/components/animation-component.hpp"
#include "game/controls.hpp"
#include "game/ecoregion.hpp"
#include "game/menu.hpp"
#include "game/states/extras-menu-state.hpp"
#include "game/states/nuptial-flight-state.hpp"
#include "game/states/options-menu-state.hpp"
#include "game/states/experiments/test-state.hpp"
#include "game/strings.hpp"
#include "game/world.hpp"
#include "game/screen-transition.hpp"
#include "game/debug/shell.hpp"
#include <engine/math/vector.hpp>
#include <engine/math/projection.hpp>
#include <engine/physics/light/exposure.hpp>
#include <engine/render/model.hpp>
#include <engine/render/passes/sky-pass.hpp>
#include <engine/resources/resource-manager.hpp>
#include <engine/hash/fnv1a.hpp>
#include <engine/render/passes/material-pass.hpp>
#include <engine/input/clipboard-events.hpp>
#include <engine/input/keyboard-events.hpp>
#include <format>
#include <limits>
#include <print>

main_menu_state::main_menu_state(::game& ctx, bool fade_in):
	game_state(ctx)
{
	debug::log_trace("Entering main menu state...");

	// Add camera to scene
	ctx.exterior_scene->add_object(*ctx.exterior_camera);
	
	const math::fvec2 viewport_size = math::fvec2(ctx.window->get_viewport_size());
	const math::fvec2 viewport_center = viewport_size * 0.5f;
	
	// Construct title text
	title_text = std::make_unique<scene::text>();
	title_text->set_material(ctx.title_font_material);
	title_text->set_color({1.0f, 1.0f, 1.0f, (fade_in) ? 1.0f : 0.0f});
	title_text->set_font(ctx.title_font);
	title_text->set_content(get_string(ctx, "title_antkeeper"));
	const auto& title_aabb = title_text->get_bounds();
	float title_w = title_aabb.max.x() - title_aabb.min.x();
	float title_h = title_aabb.max.y() - title_aabb.min.y();
	title_text->set_translation({std::round(viewport_center.x() - title_w * 0.5f), std::round(viewport_center.y() - title_h * 0.5f + (viewport_size.y() / 3.0f) / 2.0f), 0.0f});
	
	// Add text to UI
	ctx.ui_canvas->get_scene().add_object(*title_text);
	
	// Construct title fade in animation
	{
		m_title_fade_in_sequence = std::make_shared<animation_sequence>();
		auto& opacity_track = m_title_fade_in_sequence->tracks()["opacity"];
		auto& opacity_channel = opacity_track.channels().emplace_back();
		opacity_channel.keyframes().emplace(0.0f, 0.0f);
		opacity_channel.keyframes().emplace(config::menu_fade_in_duration, 1.0f);
		opacity_track.output() = [&](auto samples, auto&)
		{
			title_text->set_color({1.0f, 1.0f, 1.0f, samples[0]});
		};
	}

	// Construct title fade out animation
	{
		m_title_fade_out_sequence = std::make_shared<animation_sequence>();
		auto& opacity_track = m_title_fade_out_sequence->tracks()["opacity"];
		auto& opacity_channel = opacity_track.channels().emplace_back();
		opacity_channel.keyframes().emplace(0.0f, 1.0f);
		opacity_channel.keyframes().emplace(config::menu_fade_out_duration, 0.0f);
		opacity_track.output() = [&](auto samples, auto&)
		{
			title_text->set_color({1.0f, 1.0f, 1.0f, samples[0]});
		};
	}

	// Constructs title entity
	m_title_entity = ctx.entity_registry->create();
	ctx.entity_registry->emplace<animation_component>(m_title_entity);
	
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
					ctx.state_machine.emplace(std::make_unique<test_state>(ctx));
				}
			);
		};
		
		// Fade out title
		this->fade_out_title();
		
		// Fade out menu
		::menu::fade_out(ctx, nullptr);
		
		// Start fade out to white
		fade_out_to(ctx, change_state);
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
		fade_out_to(ctx, [&ctx](){ctx.closed=true;});
		
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
		fade_in_to(ctx, nullptr);
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
	ctx.exterior_camera->set_exposure_value(ev100_sunny16);
	
	// Setup and enable sky and ground passes
	ctx.sky_pass->set_enabled(false);
	
	// Setup window resized callback
	window_resized_subscription = ctx.window->get_resized_channel().subscribe
	(
		[&](const auto& event)
		{
			const math::fvec2 viewport_size = math::fvec2(event.window->get_viewport_size());
			const math::fvec2 viewport_center = viewport_size * 0.5f;
			
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
	
	debug::log_trace("Entered main menu state");
}

main_menu_state::~main_menu_state()
{
	debug::log_trace("Exiting main menu state...");

	//ctx.exterior_scene->remove_object(*ctx.exterior_camera);
	
	// Destruct menu
	::disable_menu_controls(ctx);
	::menu::clear_callbacks(ctx);
	::menu::remove_text_from_ui(ctx);
	::menu::delete_text(ctx);
	
	// Hide menu BG
	//ctx.menu_bg_billboard->set_active(false);
	
	// Destruct title entity
	ctx.entity_registry->destroy(m_title_entity);
	
	// Destruct text
	ctx.ui_canvas->get_scene().remove_object(*title_text);
	
	debug::log_trace("Exited main menu state");
}

void main_menu_state::fade_in_title()
{
	auto& title_player = ctx.entity_registry->get<animation_component>(m_title_entity).player;
	title_player.rewind();
	title_player.play(m_title_fade_in_sequence);
}

void main_menu_state::fade_out_title()
{
	auto& title_player = ctx.entity_registry->get<animation_component>(m_title_entity).player;
	title_player.rewind();
	title_player.play(m_title_fade_out_sequence);
}

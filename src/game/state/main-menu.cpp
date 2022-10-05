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

#include "game/state/main-menu.hpp"
#include "game/state/options-menu.hpp"
#include "game/state/extras-menu.hpp"
#include "game/state/nuptial-flight.hpp"
#include "game/world.hpp"
#include "game/load.hpp"
#include "game/menu.hpp"
#include "game/ant/swarm.hpp"
#include "render/passes/clear-pass.hpp"
#include "render/passes/ground-pass.hpp"
#include "render/passes/sky-pass.hpp"
#include "resources/resource-manager.hpp"
#include "render/model.hpp"
#include "animation/animation.hpp"
#include "animation/animator.hpp"
#include "animation/screen-transition.hpp"
#include "animation/ease.hpp"
#include "application.hpp"
#include "config.hpp"
#include "physics/light/exposure.hpp"
#include "entity/components/model.hpp"
#include "entity/components/steering.hpp"
#include "entity/components/transform.hpp"
#include <limits>

namespace game {
namespace state {

main_menu::main_menu(game::context& ctx, bool fade_in):
	game::state::base(ctx)
{
	ctx.logger->push_task("Entering main menu state");
	
	ctx.ui_clear_pass->set_cleared_buffers(true, true, false);
	
	// Construct title text
	title_text.set_material(&ctx.title_font_material);
	title_text.set_color({1.0f, 1.0f, 1.0f, (fade_in) ? 1.0f : 0.0f});
	title_text.set_font(&ctx.title_font);
	title_text.set_content((*ctx.strings)["title_antkeeper"]);
	
	// Align title text
	const auto& title_aabb = static_cast<const geom::aabb<float>&>(title_text.get_local_bounds());
	float title_w = title_aabb.max_point.x - title_aabb.min_point.x;
	float title_h = title_aabb.max_point.y - title_aabb.min_point.y;
	title_text.set_translation({std::round(-title_w * 0.5f), std::round(-title_h * 0.5f + (ctx.app->get_viewport_dimensions().y / 3.0f) / 2.0f), 0.0f});
	title_text.update_tweens();
	
	// Add title text to UI
	ctx.ui_scene->add_object(&title_text);
	
	// Construct title fade animation
	title_fade_animation.set_interpolator(ease<float>::out_cubic);
	animation_channel<float>* opacity_channel = title_fade_animation.add_channel(0);
	title_fade_animation.set_frame_callback
	(
		[this, &ctx](int channel, const float& opacity)
		{
			float4 color = this->title_text.get_color();
			color[3] = opacity;
			this->title_text.set_color(color);
		}
	);
	ctx.animator->add_animation(&title_fade_animation);
	
	// Construct menu item texts
	scene::text* start_text = new scene::text();
	scene::text* options_text = new scene::text();
	scene::text* extras_text = new scene::text();
	scene::text* quit_text = new scene::text();
	
	// Build list of menu item texts
	ctx.menu_item_texts.push_back({start_text, nullptr});
	ctx.menu_item_texts.push_back({options_text, nullptr});
	ctx.menu_item_texts.push_back({extras_text, nullptr});
	ctx.menu_item_texts.push_back({quit_text, nullptr});
	
	// Set content of menu item texts
	start_text->set_content((*ctx.strings)["main_menu_start"]);
	options_text->set_content((*ctx.strings)["main_menu_options"]);
	extras_text->set_content((*ctx.strings)["main_menu_extras"]);
	quit_text->set_content((*ctx.strings)["main_menu_quit"]);
	
	// Init menu item index
	game::menu::init_menu_item_index(ctx, "main");
	
	game::menu::update_text_color(ctx);
	game::menu::update_text_font(ctx);
	game::menu::align_text(ctx, true, false, (-ctx.app->get_viewport_dimensions().y / 3.0f) / 2.0f);
	game::menu::update_text_tweens(ctx);
	game::menu::add_text_to_ui(ctx);
	game::menu::setup_animations(ctx);
	
	auto select_start_callback = [this, &ctx]()
	{
		// Disable controls and menu callbacks
		game::menu::clear_controls(ctx);
		game::menu::clear_callbacks(ctx);
		
		// Create change state function
		auto change_state_nuptial_flight = [&ctx]()
		{
			// Queue change to nuptial state
			ctx.function_queue.push
			(
				[&ctx]()
				{
					ctx.state_machine.pop();
					ctx.state_machine.emplace(new game::state::nuptial_flight(ctx));
				}
			);
		};
		
		// Fade out title
		this->fade_out_title();
		
		// Fade out menu
		game::menu::fade_out(ctx, nullptr);
		
		// Start fade out to white
		ctx.fade_transition_color->set_value({1, 1, 1});
		ctx.fade_transition->transition(config::new_colony_fade_out_duration, false, ease<float>::out_cubic, false, change_state_nuptial_flight);
	};
	auto select_options_callback = [this, &ctx]()
	{
		game::menu::clear_controls(ctx);
		
		// Fade out title
		this->fade_out_title();
		
		// Fade out menu
		game::menu::fade_out
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
						ctx.state_machine.emplace(new game::state::options_menu(ctx));
					}
				);
			}
		);
	};
	auto select_extras_callback = [this, &ctx]()
	{
		// Disable controls
		game::menu::clear_controls(ctx);
		
		// Fade out title
		this->fade_out_title();
		
		// Fade out menu
		game::menu::fade_out
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
						ctx.state_machine.emplace(new game::state::extras_menu(ctx));
					}
				);
			}
		);
	};
	auto select_quit_callback = [this, &ctx]()
	{
		// Disable controls
		game::menu::clear_controls(ctx);
		
		// Fade out title
		this->fade_out_title();
		
		// Fade out menu
		game::menu::fade_out(ctx, nullptr);
		
		// Fade to black then quit
		ctx.fade_transition->transition(config::quit_fade_out_duration, false, ease<float>::out_cubic, false, std::bind(&application::close, ctx.app));
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
	
	// Queue menu control setup
	ctx.function_queue.push(std::bind(game::menu::setup_controls, std::ref(ctx)));
	
	if (fade_in)
	{
		// Fade in from black
		ctx.fade_transition->transition(config::title_fade_in_duration, true, ease<float>::out_cubic);
	}
	else
	{
		// Fade in text
		fade_in_title();
		game::menu::fade_in(ctx, nullptr);
	}
	
	if (ctx.entities.find("earth") == ctx.entities.end())
	{
		game::world::cosmogenesis(ctx);
		game::world::create_observer(ctx);
		game::load::biome(ctx, "debug.bio");
	}
	
	// Set world time
	game::world::set_time(ctx, 2022, 6, 21, 12, 0, 0.0);
	
	// Set world time scale
	game::world::set_time_scale(ctx, 0.0);
	
	ctx.surface_camera->set_active(true);
	const float ev100_sunny16 = physics::light::ev::from_settings(16.0f, 1.0f / 100.0f, 100.0f);
	ctx.surface_camera->set_exposure(15.5f);
	
	ctx.surface_camera->look_at({-55, 36, 10}, {0, 100, 0}, {0, 1, 0});
	ctx.surface_camera->update_tweens();
	
	// Setup and enable sky and ground passes
	ctx.sky_pass->set_enabled(true);
	ctx.ground_pass->set_enabled(true);
	
	// Disable UI color clear
	ctx.ui_clear_pass->set_cleared_buffers(false, true, false);
	
	// Create mating swarm
	game::ant::create_swarm(ctx);
	
	if (!ctx.menu_bg_billboard->is_active())
		game::menu::fade_in_bg(ctx);
		
	
	ctx.logger->pop_task(EXIT_SUCCESS);
}

main_menu::~main_menu()
{
	ctx.logger->push_task("Exiting main menu state");
	
	// Destruct menu
	game::menu::clear_controls(ctx);
	game::menu::clear_callbacks(ctx);
	game::menu::delete_animations(ctx);
	game::menu::remove_text_from_ui(ctx);
	game::menu::delete_text(ctx);
	
	// Destruct title animation
	ctx.animator->remove_animation(&title_fade_animation);
	
	// Destruct title text
	ctx.ui_scene->remove_object(&title_text);
	
	// Destroy swarm
	game::ant::destroy_swarm(ctx);
	
	ctx.logger->pop_task(EXIT_SUCCESS);
}

void main_menu::fade_in_title()
{
	animation_channel<float>* opacity_channel = title_fade_animation.get_channel(0);
	opacity_channel->remove_keyframes();
	opacity_channel->insert_keyframe({0.0, 0.0f});
	opacity_channel->insert_keyframe({config::menu_fade_in_duration, 1.0f});
	title_fade_animation.stop();
	title_fade_animation.play();
}

void main_menu::fade_out_title()
{
	animation_channel<float>* opacity_channel = title_fade_animation.get_channel(0);
	opacity_channel->remove_keyframes();
	opacity_channel->insert_keyframe({0.0, 1.0f});
	opacity_channel->insert_keyframe({config::menu_fade_out_duration, 0.0f});
	title_fade_animation.stop();
	title_fade_animation.play();
}

} // namespace state
} // namespace game

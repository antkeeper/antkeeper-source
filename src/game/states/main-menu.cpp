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
#include "game/states/options-menu.hpp"
#include "game/states/extras-menu.hpp"
#include "game/states/forage.hpp"
#include "game/states/nuptial-flight.hpp"
#include "game/menu.hpp"
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

static void fade_in_title(game::context* ctx)
{
	ctx->title_fade_animation->set_interpolator(ease<float>::out_cubic);
	animation_channel<float>* opacity_channel = ctx->title_fade_animation->get_channel(0);
	opacity_channel->remove_keyframes();
	opacity_channel->insert_keyframe({0.0, 0.0f});
	opacity_channel->insert_keyframe({game::menu::fade_in_duration, 1.0f});
	ctx->title_fade_animation->stop();
	ctx->title_fade_animation->play();
}

static void fade_out_title(game::context* ctx)
{
	ctx->title_fade_animation->set_interpolator(ease<float>::out_cubic);
	animation_channel<float>* opacity_channel = ctx->title_fade_animation->get_channel(0);
	opacity_channel->remove_keyframes();
	opacity_channel->insert_keyframe({0.0, 1.0f});
	opacity_channel->insert_keyframe({game::menu::fade_out_duration, 0.0f});
	ctx->title_fade_animation->stop();
	ctx->title_fade_animation->play();
}

void enter(game::context* ctx, bool fade_in)
{
	ctx->ui_clear_pass->set_cleared_buffers(true, true, false);
	
	// Construct title text
	ctx->title_text = new scene::text();
	ctx->title_text->set_material(&ctx->title_font_material);
	ctx->title_text->set_font(&ctx->title_font);
	ctx->title_text->set_color({1.0f, 1.0f, 1.0f, 1.0f});
	ctx->title_text->set_content((*ctx->strings)["title_antkeeper"]);
	
	// Align title text
	const auto& title_aabb = static_cast<const geom::aabb<float>&>(ctx->title_text->get_local_bounds());
	float title_w = title_aabb.max_point.x - title_aabb.min_point.x;
	float title_h = title_aabb.max_point.y - title_aabb.min_point.y;
	ctx->title_text->set_translation({std::round(-title_w * 0.5f), std::round(-title_h * 0.5f + (std::get<1>(ctx->app->get_viewport_dimensions()) / 3.0f) / 2.0f), 0.0f});
	ctx->title_text->update_tweens();
	
	// Add title text to UI
	ctx->ui_scene->add_object(ctx->title_text);
	
	// Construct title fade animation
	ctx->title_fade_animation = new animation<float>();
	animation_channel<float>* opacity_channel = ctx->title_fade_animation->add_channel(0);
	
	ctx->title_fade_animation->set_frame_callback
	(
		[ctx](int channel, const float& opacity)
		{
			float4 color = ctx->title_text->get_color();
			color[3] = opacity;
			ctx->title_text->set_color(color);
		}
	);
	ctx->animator->add_animation(ctx->title_fade_animation);
	
	// Construct menu item texts
	scene::text* start_text = new scene::text();
	scene::text* options_text = new scene::text();
	scene::text* extras_text = new scene::text();
	scene::text* quit_text = new scene::text();
	
	// Build list of menu item texts
	ctx->menu_item_texts.push_back({start_text, nullptr});
	ctx->menu_item_texts.push_back({options_text, nullptr});
	ctx->menu_item_texts.push_back({extras_text, nullptr});
	ctx->menu_item_texts.push_back({quit_text, nullptr});
	
	// Set content of menu item texts
	start_text->set_content((*ctx->strings)["main_menu_start"]);
	options_text->set_content((*ctx->strings)["main_menu_options"]);
	extras_text->set_content((*ctx->strings)["main_menu_extras"]);
	quit_text->set_content((*ctx->strings)["main_menu_quit"]);
	
	// Init menu item index
	game::menu::init_menu_item_index(ctx, "main");
	
	game::menu::update_text_color(ctx);
	game::menu::update_text_font(ctx);
	game::menu::align_text(ctx, true, false, (-std::get<1>(ctx->app->get_viewport_dimensions()) / 3.0f) / 2.0f);
	game::menu::update_text_tweens(ctx);
	game::menu::add_text_to_ui(ctx);
	game::menu::setup_animations(ctx);
	
	auto select_start_callback = [ctx]()
	{
		// Disable controls and menu callbacks
		game::menu::clear_controls(ctx);
		game::menu::clear_callbacks(ctx);
		
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
		ctx->fade_transition->transition(fade_out_duration, false, ease<float>::out_cubic, false);
	};
	auto select_options_callback = [ctx]()
	{
		game::menu::clear_controls(ctx);
		
		// Fade out title
		fade_out_title(ctx);
		
		// Fade out menu
		game::menu::fade_out
		(
			ctx,
			[ctx]()
			{
				application::state next_state;
				next_state.name = "options_menu";
				next_state.enter = std::bind(game::state::options_menu::enter, ctx);
				next_state.exit = std::bind(game::state::options_menu::exit, ctx);
				ctx->app->queue_state(next_state);
			}
		);
	};
	auto select_extras_callback = [ctx]()
	{
		// Disable controls
		game::menu::clear_controls(ctx);
		
		// Fade out title
		fade_out_title(ctx);
		
		// Fade out menu
		game::menu::fade_out
		(
			ctx,
			[ctx]()
			{
				application::state next_state;
				next_state.name = "extras_menu";
				next_state.enter = std::bind(game::state::extras_menu::enter, ctx);
				next_state.exit = std::bind(game::state::extras_menu::exit, ctx);
				ctx->app->queue_state(next_state);
			}
		);
	};
	auto select_quit_callback = [ctx]()
	{
		ctx->app->close(EXIT_SUCCESS);
	};
	
	// Build list of menu select callbacks
	ctx->menu_select_callbacks.push_back(select_start_callback);
	ctx->menu_select_callbacks.push_back(select_options_callback);
	ctx->menu_select_callbacks.push_back(select_extras_callback);
	ctx->menu_select_callbacks.push_back(select_quit_callback);
	
	// Build list of menu left callbacks
	ctx->menu_left_callbacks.push_back(nullptr);
	ctx->menu_left_callbacks.push_back(nullptr);
	ctx->menu_left_callbacks.push_back(nullptr);
	ctx->menu_left_callbacks.push_back(nullptr);
	
	// Build list of menu right callbacks
	ctx->menu_right_callbacks.push_back(nullptr);
	ctx->menu_right_callbacks.push_back(nullptr);
	ctx->menu_right_callbacks.push_back(nullptr);
	ctx->menu_right_callbacks.push_back(nullptr);
	
	// Set menu back callback
	ctx->menu_back_callback = select_quit_callback;
	
	// Schedule menu control setup
	timeline* timeline = ctx->timeline;
	float t = timeline->get_position();
	timeline->add_sequence({{t + game::menu::input_delay, std::bind(game::menu::setup_controls, ctx)}});
	
	if (fade_in)
	{
		ctx->fade_transition->transition(0.5f, true, ease<float>::out_cubic);
	}
	else
	{
		// Fade in title
		ctx->title_text->set_color({1.0f, 1.0f, 1.0f, 0.0f});
		ctx->title_text->update_tweens();
		fade_in_title(ctx);
		
		// Fade in menu
		game::menu::fade_in(ctx, nullptr);
	}
}

void exit(game::context* ctx)
{	
	// Destruct menu
	game::menu::clear_controls(ctx);
	game::menu::clear_callbacks(ctx);
	game::menu::delete_animations(ctx);
	game::menu::remove_text_from_ui(ctx);
	game::menu::delete_text(ctx);
	
	// Destruct title animation
	ctx->animator->remove_animation(ctx->title_fade_animation);
	delete ctx->title_fade_animation;
	ctx->title_fade_animation = nullptr;
	
	// Destruct title text
	ctx->ui_scene->remove_object(ctx->title_text);
	delete ctx->title_text;
	ctx->title_text = nullptr;
}

} // namespace main_menu
} // namespace state
} // namespace game

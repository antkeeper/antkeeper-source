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
#include "animation/timeline.hpp"
#include "application.hpp"
#include "scene/text.hpp"
#include "configuration.hpp"

namespace game {
namespace state {
namespace title {

void enter(game::context* ctx)
{
	// Setup timing
	const float title_fade_in_duration = 0.5f;
	const float title_fade_out_duration = 0.5f;
	
	// Start fade in
	ctx->fade_transition->transition(title_fade_in_duration, true, ease<float>::in_quad);
	
	// Crate fade out function
	auto fade_out = [ctx, title_fade_out_duration]()
	{
		ctx->fade_transition->transition(title_fade_out_duration, false, ease<float>::out_quad);
	};
	
	// Create change state function
	auto change_state = [ctx]()
	{
		application::state next_state;
		next_state.name = "main_menu";
		next_state.enter = std::bind(game::state::main_menu::enter, ctx);
		next_state.exit = std::bind(game::state::main_menu::exit, ctx);
		
		ctx->app->change_state(next_state);
	};
	
	// Set up title skipper
	ctx->input_listener->set_callback
	(
		[ctx](const event_base& event)
		{
			auto id = event.get_event_type_id();
			if (id != mouse_moved_event::event_type_id && id != mouse_wheel_scrolled_event::event_type_id && id != gamepad_axis_moved_event::event_type_id)
			{
				ctx->timeline->clear();
				ctx->fade_transition->get_animation()->stop();
				ctx->rasterizer->set_clear_color(0.0f, 0.0f, 0.0f, 1.0f);
				ctx->rasterizer->clear_framebuffer(true, false, false);
				ctx->app->swap_buffers();
				
				application::state next_state;
				next_state.name = "main_menu";
				next_state.enter = std::bind(game::state::main_menu::enter, ctx);
				next_state.exit = std::bind(game::state::main_menu::exit, ctx);
				
				ctx->app->change_state(next_state);
			}
		}
	);
	ctx->input_listener->set_enabled(true);
	
	// Construct title text
	ctx->title_text = new scene::text();
	ctx->title_text->set_material(&ctx->title_font_material);
	ctx->title_text->set_font(&ctx->title_font);
	ctx->title_text->set_color({1.0f, 1.0f, 1.0f, 1.0f});
	ctx->title_text->set_content((*ctx->strings)["title"]);
	ctx->ui_scene->add_object(ctx->title_text);
	
	// Construct version string text
	ctx->title_version_text = new scene::text();
	ctx->title_version_text->set_material(&ctx->debug_font_material);
	ctx->title_version_text->set_font(&ctx->debug_font);
	ctx->title_version_text->set_color({1.0f, 1.0f, 1.0f, 1.0f});
	ctx->title_version_text->set_content(ANTKEEPER_VERSION_STRING);
	ctx->ui_scene->add_object(ctx->title_version_text);
	
	// Align title text
	const auto& title_aabb = static_cast<const geom::aabb<float>&>(ctx->title_text->get_local_bounds());
	float title_w = title_aabb.max_point.x - title_aabb.min_point.x;
	float title_h = title_aabb.max_point.y - title_aabb.min_point.y;
	ctx->title_text->set_translation({std::round(-title_w * 0.5f), std::round(-title_h * 0.5f), 0.0f});
	
	// Align version string
	const auto& version_aabb = static_cast<const geom::aabb<float>&>(ctx->title_version_text->get_local_bounds());
	float version_w = version_aabb.max_point.x - version_aabb.min_point.x;
	float version_h = version_aabb.max_point.y - version_aabb.min_point.y;
	const float version_padding = 12.0f;
	auto viewport = ctx->app->get_viewport_dimensions();
	ctx->title_version_text->set_translation({viewport[0] * 0.5f - version_w - version_padding, -viewport[1] * 0.5f + version_padding, 0.0f});
}

void exit(game::context* ctx)
{
	// Remove and destruct title and version text
	ctx->ui_scene->remove_object(ctx->title_text);
	ctx->ui_scene->remove_object(ctx->title_version_text);
	delete ctx->title_text;
	delete ctx->title_version_text;
	ctx->title_text = nullptr;
	ctx->title_version_text = nullptr;
	
	// Disable title skipper
	ctx->input_listener->set_enabled(false);
	ctx->input_listener->set_callback(nullptr);
}

} // namespace title
} // namespace state
} // namespace game

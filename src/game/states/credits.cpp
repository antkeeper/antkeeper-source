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

#include "game/states/credits.hpp"
#include "game/states/main-menu.hpp"
#include "animation/ease.hpp"
#include "animation/animation.hpp"
#include "animation/animator.hpp"
#include "application.hpp"
#include "scene/text.hpp"
#include "render/passes/clear-pass.hpp"

namespace game {
namespace state {
namespace credits {

void enter(game::context* ctx)
{
	ctx->ui_clear_pass->set_cleared_buffers(true, true, false);
	
	// Construct credits text
	ctx->credits_text = new scene::text();
	ctx->credits_text->set_material(&ctx->menu_font_material);
	ctx->credits_text->set_font(&ctx->menu_font);
	ctx->credits_text->set_color({1.0f, 1.0f, 1.0f, 0.0f});
	ctx->credits_text->set_content((*ctx->strings)["credits"]);
	
	// Align credits text
	const auto& credits_aabb = static_cast<const geom::aabb<float>&>(ctx->credits_text->get_local_bounds());
	float credits_w = credits_aabb.max_point.x - credits_aabb.min_point.x;
	float credits_h = credits_aabb.max_point.y - credits_aabb.min_point.y;
	ctx->credits_text->set_translation({std::round(-credits_w * 0.5f), std::round(-credits_h * 0.5f), 0.0f});
	
	// Load animation timing configuration
	double credits_fade_in_duration = 0.0;
	double credits_scroll_duration = 0.0;
	if (ctx->config->contains("credits_fade_in_duration"))
		credits_fade_in_duration = (*ctx->config)["credits_fade_in_duration"].get<double>();
	if (ctx->config->contains("credits_scroll_duration"))
		credits_scroll_duration = (*ctx->config)["credits_scroll_duration"].get<double>();
	
	auto set_credits_opacity = [ctx](int channel, const float& opacity)
	{
		ctx->credits_text->set_color({1.0f, 1.0f, 1.0f, opacity});
	};
	
	// Build credits fade in animation
	ctx->credits_fade_in_animation = new animation<float>();
	animation_channel<float>* credits_fade_in_opacity_channel = ctx->credits_fade_in_animation->add_channel(0);
	ctx->credits_fade_in_animation->set_interpolator(ease<float>::in_quad);
	credits_fade_in_opacity_channel->insert_keyframe({0.0, 0.0f});
	credits_fade_in_opacity_channel->insert_keyframe({credits_fade_in_duration, 1.0f});
	ctx->credits_fade_in_animation->set_frame_callback(set_credits_opacity);
	
	// Build credits scroll in animation
	ctx->credits_scroll_animation = new animation<float>();
	
	// Trigger credits scroll animation after credits fade in animation ends
	ctx->credits_fade_in_animation->set_end_callback
	(
		[ctx]()
		{
			ctx->credits_scroll_animation->play();
		}
	);
	
	// Add credits animations to animator
	ctx->animator->add_animation(ctx->credits_fade_in_animation);
	ctx->animator->add_animation(ctx->credits_scroll_animation);
	
	// Start credits fade in animation
	ctx->credits_fade_in_animation->play();
	
	// Set up credits skipper
	ctx->input_listener->set_callback
	(
		[ctx](const event_base& event)
		{
			auto id = event.get_event_type_id();
			if (id != mouse_moved_event::event_type_id && id != mouse_wheel_scrolled_event::event_type_id && id != gamepad_axis_moved_event::event_type_id)
			{	
				if (ctx->credits_text->get_color()[3] > 0.0f)
				{
					ctx->input_listener->set_enabled(false);
					
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
	
	ctx->ui_scene->add_object(ctx->credits_text);
	ctx->credits_text->update_tweens();
}

void exit(game::context* ctx)
{
	// Disable credits skipper
	ctx->input_listener->set_enabled(false);
	ctx->input_listener->set_callback(nullptr);
	
	// Destruct credits text
	ctx->ui_scene->remove_object(ctx->credits_text);
	delete ctx->credits_text;
	ctx->credits_text = nullptr;
	
	// Destruct credits animations
	ctx->animator->remove_animation(ctx->credits_fade_in_animation);
	ctx->animator->remove_animation(ctx->credits_scroll_animation);
	delete ctx->credits_fade_in_animation;
	delete ctx->credits_scroll_animation;
	ctx->credits_fade_in_animation = nullptr;
	ctx->credits_scroll_animation = nullptr;
	
	ctx->ui_clear_pass->set_cleared_buffers(false, true, false);
}

} // namespace credits
} // namespace state
} // namespace game

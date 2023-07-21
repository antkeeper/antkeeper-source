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

#include "game/states/credits-state.hpp"
#include "game/states/extras-menu-state.hpp"
#include "game/game.hpp"
#include <engine/animation/ease.hpp>
#include <engine/animation/animation.hpp>
#include <engine/animation/animator.hpp>
#include <engine/scene/text.hpp>
#include <engine/debug/log.hpp>
#include "game/strings.hpp"
#include <engine/utility/hash/fnv1a.hpp>
#include <engine/math/vector.hpp>

credits_state::credits_state(::game& ctx):
	game_state(ctx)
{
	debug::log::trace("Entering credits state...");
	
	const math::fvec2 viewport_size = math::fvec2(ctx.window->get_viewport_size());
	const math::fvec2 viewport_center = viewport_size * 0.5f;
	
	// Construct credits text
	credits_text.set_material(ctx.menu_font_material);
	credits_text.set_font(&ctx.menu_font);
	credits_text.set_color({1.0f, 1.0f, 1.0f, 0.0f});
	credits_text.set_content(get_string(ctx, "credits"));
	
	// Align credits text
	const auto& credits_aabb = credits_text.get_bounds();
	float credits_w = credits_aabb.max.x() - credits_aabb.min.x();
	float credits_h = credits_aabb.max.y() - credits_aabb.min.y();
	credits_text.set_translation({std::round(viewport_center.x() - credits_w * 0.5f), std::round(viewport_center.y() - credits_h * 0.5f), 0.0f});
	
	// Set up animation timing configuration
	const float credits_fade_in_duration = 0.5;
	const float credits_scroll_duration = 5.0;
	
	auto set_credits_opacity = [this](int channel, const float& opacity)
	{
		this->credits_text.set_color({1.0f, 1.0f, 1.0f, opacity});
	};
	
	// Build credits fade in animation
	credits_fade_in_animation.set_interpolator(ease<float>::in_quad);
	animation_channel<float>* credits_fade_in_opacity_channel = credits_fade_in_animation.add_channel(0);
	credits_fade_in_opacity_channel->insert_keyframe({0.0f, 0.0f});
	credits_fade_in_opacity_channel->insert_keyframe({credits_fade_in_duration, 1.0f});
	credits_fade_in_animation.set_frame_callback(set_credits_opacity);
	
	// Add credits animations to animator
	ctx.animator->add_animation(&credits_fade_in_animation);
	
	// Start credits fade in animation
	credits_fade_in_animation.play();
	
	// Setup window resized callback
	window_resized_subscription = ctx.window->get_resized_channel().subscribe
	(
		[&](const auto& event)
		{
			const math::fvec2 viewport_size = math::fvec2(event.window->get_viewport_size());
			const math::fvec2 viewport_center = viewport_size * 0.5f;
			const auto& credits_aabb = credits_text.get_bounds();
			float credits_w = credits_aabb.max.x() - credits_aabb.min.x();
			float credits_h = credits_aabb.max.y() - credits_aabb.min.y();
			credits_text.set_translation({std::round(viewport_center.x() - credits_w * 0.5f), std::round(viewport_center.y() - credits_h * 0.5f), 0.0f});
		}
	);
	
	// Construct credits skip function
	auto skip = [&](const auto& event)
	{
		ctx.function_queue.emplace
		(
			[&]()
			{
				// Change to extras menu state
				ctx.state_machine.pop();
				ctx.state_machine.emplace(std::make_unique<extras_menu_state>(ctx));
			}
		);
	};
	
	// Set up credits skippers
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
	
	// Enable credits skippers next frame
	ctx.function_queue.push
	(
		[&]()
		{
			ctx.input_mapper.connect(ctx.input_manager->get_event_dispatcher());
		}
	);
	
	ctx.ui_scene->add_object(credits_text);
	
	debug::log::trace("Entered credits state");
}

credits_state::~credits_state()
{
	debug::log::trace("Exiting credits state...");
	
	// Disable credits skippers
	ctx.input_mapper.disconnect();
	input_mapped_subscriptions.clear();
	
	// Destruct credits text
	ctx.ui_scene->remove_object(credits_text);
	
	// Destruct credits animations
	ctx.animator->remove_animation(&credits_fade_in_animation);
	
	debug::log::trace("Exited credits state");
}

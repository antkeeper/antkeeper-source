// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include "game/states/credits-state.hpp"
#include "game/states/extras-menu-state.hpp"
#include "game/game.hpp"
#include <engine/animation/ease.hpp>
#include <engine/scene/text.hpp>
#include <engine/debug/log.hpp>
#include "game/strings.hpp"
#include "game/components/animation-component.hpp"
#include <engine/hash/fnv.hpp>
#include <engine/math/vector.hpp>

credits_state::credits_state(::game& ctx):
	game_state(ctx)
{
	debug::log_trace("Entering credits state...");
	
	const math::fvec2 viewport_size = math::fvec2(ctx.window->get_viewport_size());
	const math::fvec2 viewport_center = viewport_size * 0.5f;
	
	// Construct credits text
	credits_text.set_material(ctx.menu_font_material);
	credits_text.set_font(ctx.menu_font);
	credits_text.set_color({1.0f, 1.0f, 1.0f, 0.0f});
	credits_text.set_content(get_string(ctx, "credits"));
	
	// Align credits text
	const auto& credits_aabb = credits_text.get_bounds();
	float credits_w = credits_aabb.max.x() - credits_aabb.min.x();
	float credits_h = credits_aabb.max.y() - credits_aabb.min.y();
	credits_text.set_translation({std::round(viewport_center.x() - credits_w * 0.5f), std::round(viewport_center.y() - credits_h * 0.5f), 0.0f});
	
	// Set up animation timing configuration
	const float credits_fade_in_duration = 0.5;
	
	// Construct roll credits sequence
	{
		m_roll_credits_sequence = std::make_shared<animation_sequence>();
		auto& opacity_track = m_roll_credits_sequence->tracks()["opacity"];
		auto& opacity_channel = opacity_track.channels().emplace_back();
		opacity_channel.keyframes().emplace(0.0f, 0.0f);
		opacity_channel.keyframes().emplace(credits_fade_in_duration, 1.0f);
		opacity_track.output() = [&](auto samples, auto&)
		{
			credits_text.set_color({1.0f, 1.0f, 1.0f, samples[0]});
		};
	}
	
	// Constructs credits entity
	m_credits_entity = ctx.entity_registry->create();
	ctx.entity_registry->emplace<animation_component>(m_credits_entity);
	
	// Start credits roll
	auto& credits_player = ctx.entity_registry->get<animation_component>(m_credits_entity).player;
	credits_player.play(m_roll_credits_sequence);
	
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
	auto skip = [&]([[maybe_unused]] const auto& event)
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
	
	ctx.ui_canvas->get_scene().add_object(credits_text);
	
	debug::log_trace("Entered credits state");
}

credits_state::~credits_state()
{
	debug::log_trace("Exiting credits state...");
	
	// Disable credits skippers
	ctx.input_mapper.disconnect();
	input_mapped_subscriptions.clear();
	
	// Destruct credits text
	ctx.entity_registry->destroy(m_credits_entity);
	ctx.ui_canvas->get_scene().remove_object(credits_text);
	
	debug::log_trace("Exited credits state");
}

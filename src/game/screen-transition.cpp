// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include "game/screen-transition.hpp"
#include "game/game.hpp"
#include "game/components/animation-component.hpp"

void fade_out_to(::game& ctx, const std::function<void()>& callback)
{
	ctx.screen_fade_out_sequence->cues().clear();

	ctx.screen_fade_out_sequence->cues().emplace(0.0f, [&](auto&)
	{
		ctx.screen_transition_billboard->set_layer_mask(1);
	});

	ctx.screen_fade_out_sequence->cues().emplace(ctx.screen_fade_out_sequence->duration() + 0.1f, [&, callback](auto&)
	{
		ctx.screen_transition_billboard->set_layer_mask(0);
		if (callback)
		{
			callback();
		}
	});

	// Start credits roll
	auto& player = ctx.entity_registry->get<animation_component>(ctx.screen_transition_entity).player;
	player.rewind();
	player.play(ctx.screen_fade_out_sequence);
}

void fade_in_to(::game& ctx, const std::function<void()>& callback)
{
	ctx.screen_fade_in_sequence->cues().clear();

	ctx.screen_fade_in_sequence->cues().emplace(0.0f, [&](auto&)
	{
		ctx.screen_transition_billboard->set_layer_mask(1);
	});

	ctx.screen_fade_in_sequence->cues().emplace(ctx.screen_fade_in_sequence->duration() + 0.1f, [&, callback](auto&)
	{
		ctx.screen_transition_billboard->set_layer_mask(0);
		if (callback)
		{
			callback();
		}
	});

	// Start credits roll
	auto& player = ctx.entity_registry->get<animation_component>(ctx.screen_transition_entity).player;
	player.rewind();
	player.play(ctx.screen_fade_in_sequence);
}

// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_SPLASH_STATE_HPP
#define ANTKEEPER_SPLASH_STATE_HPP

#include "game/states/game-state.hpp"
#include <engine/render/material.hpp>
#include <engine/scene/billboard.hpp>
#include <engine/animation/animation.hpp>
#include <engine/event/subscription.hpp>
#include <vector>


class splash_state: public game_state
{
public:
	explicit splash_state(::game& ctx);
	virtual ~splash_state();
	
private:
	std::shared_ptr<render::material> splash_billboard_material;
	scene::billboard splash_billboard;
	animation<float> splash_fade_in_animation;
	animation<float> splash_fade_out_animation;
	std::vector<std::shared_ptr<::event::subscription>> input_mapped_subscriptions;
	std::shared_ptr<event::subscription> window_resized_subscription;
};

#endif // ANTKEEPER_SPLASH_STATE_HPP

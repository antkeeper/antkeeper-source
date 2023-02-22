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

#ifndef ANTKEEPER_GAME_STATE_SPLASH_HPP
#define ANTKEEPER_GAME_STATE_SPLASH_HPP

#include "game/state/base.hpp"
#include <engine/render/material.hpp>
#include <engine/scene/billboard.hpp>
#include <engine/animation/animation.hpp>
#include <engine/event/subscription.hpp>
#include <vector>

namespace state {

class splash: public ::state::base
{
public:
	splash(::context& ctx);
	virtual ~splash();
	
private:
	render::material splash_billboard_material;
	scene::billboard splash_billboard;
	animation<float> splash_fade_in_animation;
	animation<float> splash_fade_out_animation;
	std::vector<std::shared_ptr<::event::subscription>> input_mapped_subscriptions;
	std::shared_ptr<event::subscription> window_resized_subscription;
};

} // namespace state

#endif // ANTKEEPER_GAME_STATE_SPLASH_HPP

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

#ifndef ANTKEEPER_CREDITS_STATE_HPP
#define ANTKEEPER_CREDITS_STATE_HPP

#include "game/states/game-state.hpp"
#include <engine/scene/text.hpp>
#include <engine/animation/animation.hpp>
#include <engine/event/subscription.hpp>
#include <vector>


class credits_state: public game_state
{
public:
	credits_state(::game& ctx);
	virtual ~credits_state();
	
private:
	scene::text credits_text;
	animation<float> credits_fade_in_animation;
	animation<float> credits_scroll_animation;
	std::vector<std::shared_ptr<::event::subscription>> input_mapped_subscriptions;
	std::shared_ptr<event::subscription> window_resized_subscription;
};

#endif // ANTKEEPER_CREDITS_STATE_HPP

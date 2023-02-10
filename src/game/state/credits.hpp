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

#ifndef ANTKEEPER_GAME_STATE_CREDITS_HPP
#define ANTKEEPER_GAME_STATE_CREDITS_HPP

#include "game/state/base.hpp"
#include "scene/text.hpp"
#include "animation/animation.hpp"
#include "event/subscription.hpp"
#include <vector>

namespace game {
namespace state {

class credits: public game::state::base
{
public:
	credits(game::context& ctx);
	virtual ~credits();
	
private:
	scene::text credits_text;
	animation<float> credits_fade_in_animation;
	animation<float> credits_scroll_animation;
	std::vector<std::shared_ptr<::event::subscription>> input_mapped_subscriptions;
	std::shared_ptr<event::subscription> window_resized_subscription;
};

} // namespace state
} // namespace game

#endif // ANTKEEPER_GAME_STATE_CREDITS_HPP

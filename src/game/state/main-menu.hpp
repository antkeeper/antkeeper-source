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

#ifndef ANTKEEPER_GAME_STATE_MAIN_MENU_HPP
#define ANTKEEPER_GAME_STATE_MAIN_MENU_HPP

#include "game/state/base.hpp"
#include <engine/scene/text.hpp>
#include <engine/animation/animation.hpp>
#include <engine/entity/id.hpp>
#include <engine/event/subscription.hpp>
#include <memory>

namespace state {

class main_menu: public ::state::base
{
public:
	main_menu(::context& ctx, bool fade_in);
	virtual ~main_menu();
	
private:
	void fade_in_title();
	void fade_out_title();
	
	scene::text title_text;
	animation<float> title_fade_animation;
	
	std::shared_ptr<event::subscription> window_resized_subscription;
};

} // namespace state

#endif // ANTKEEPER_GAME_STATE_MAIN_MENU_HPP

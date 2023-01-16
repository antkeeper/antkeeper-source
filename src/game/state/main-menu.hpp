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
#include "scene/text.hpp"
#include "animation/animation.hpp"
#include "entity/id.hpp"
#include "event/signal.hpp"

namespace game {
namespace state {

class main_menu: public game::state::base
{
public:
	main_menu(game::context& ctx, bool fade_in);
	virtual ~main_menu();
	
private:
	void fade_in_title();
	void fade_out_title();
	
	scene::text title_text;
	animation<float> title_fade_animation;
	
	entity::id swarm_eid;
	
	std::shared_ptr<connection> window_close_connection;
	std::shared_ptr<connection> window_motion_connection;
	std::shared_ptr<connection> window_focus_connection;
	std::shared_ptr<connection> viewport_size_connection;
};

} // namespace state
} // namespace game

#endif // ANTKEEPER_GAME_STATE_MAIN_MENU_HPP

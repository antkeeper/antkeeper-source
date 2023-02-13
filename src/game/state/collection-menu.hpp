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

#ifndef ANTKEEPER_GAME_STATE_COLLECTION_MENU_HPP
#define ANTKEEPER_GAME_STATE_COLLECTION_MENU_HPP

#include "game/state/base.hpp"
#include "event/subscription.hpp"
#include "render/material.hpp"
#include "scene/billboard.hpp"
#include "animation/animation.hpp"
#include "geom/primitive/rectangle.hpp"
#include <memory>

namespace game {
namespace state {

class collection_menu: public game::state::base
{
public:
	collection_menu(game::context& ctx);
	virtual ~collection_menu();
	
private:
	void resize_box();
	
	render::material selection_material;
	scene::billboard selection_billboard;
	animation<float> selection_snap_animation;
	
	render::material box_material;
	scene::billboard box_billboard;
	
	std::shared_ptr<event::subscription> mouse_moved_subscription;
	std::shared_ptr<event::subscription> mouse_button_pressed_subscription;
	std::shared_ptr<event::subscription> window_resized_subscription;
	
	geom::primitive::rectangle<float> box_bounds;
	int row_count;
	int column_count;
	int selected_row;
	int selected_column;
	float selection_size;
};

} // namespace state
} // namespace game

#endif // ANTKEEPER_GAME_STATE_COLLECTION_MENU_HPP

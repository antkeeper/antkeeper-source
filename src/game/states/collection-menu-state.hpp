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

#ifndef ANTKEEPER_COLLECTION_MENU_STATE_HPP
#define ANTKEEPER_COLLECTION_MENU_STATE_HPP

#include "game/states/game-state.hpp"
#include <engine/event/subscription.hpp>
#include <engine/render/material.hpp>
#include <engine/scene/billboard.hpp>
#include <engine/animation/animation.hpp>
#include <engine/geom/primitive/rectangle.hpp>
#include <memory>


class collection_menu_state: public game_state
{
public:
	collection_menu_state(::game& ctx);
	virtual ~collection_menu_state();
	
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

#endif // ANTKEEPER_COLLECTION_MENU_STATE_HPP

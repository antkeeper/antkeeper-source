// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_COLLECTION_MENU_STATE_HPP
#define ANTKEEPER_COLLECTION_MENU_STATE_HPP

#include "game/states/game-state.hpp"
#include <engine/event/subscription.hpp>
#include <engine/render/material.hpp>
#include <engine/scene/billboard.hpp>
#include <engine/animation/animation.hpp>
#include <engine/geom/primitives/rectangle.hpp>
#include <memory>


class collection_menu_state: public game_state
{
public:
	explicit collection_menu_state(::game& ctx);
	virtual ~collection_menu_state();
	
private:
	void resize_box();
	
	std::shared_ptr<render::material> selection_material;
	scene::billboard selection_billboard;
	
	std::shared_ptr<render::material> box_material;
	scene::billboard box_billboard;
	
	std::shared_ptr<event::subscription> mouse_moved_subscription;
	std::shared_ptr<event::subscription> mouse_button_pressed_subscription;
	std::shared_ptr<event::subscription> window_resized_subscription;
	
	geom::rectangle<float> box_bounds;
	int row_count;
	int column_count;
	int selected_row;
	int selected_column;
	float selection_size;
};

#endif // ANTKEEPER_COLLECTION_MENU_STATE_HPP

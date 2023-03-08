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

#ifndef ANTKEEPER_KEYBOARD_CONFIG_MENU_STATE_HPP
#define ANTKEEPER_KEYBOARD_CONFIG_MENU_STATE_HPP

#include "game/states/game-state.hpp"
#include <engine/input/action.hpp>
#include <engine/input/action-map.hpp>
#include <engine/event/subscription.hpp>
#include <engine/utility/hash/fnv1a.hpp>
#include <cstdint>
#include <engine/scene/text.hpp>
#include <memory>

class keyboard_config_menu_state: public game_state
{
public:
	keyboard_config_menu_state(::game& ctx);
	virtual ~keyboard_config_menu_state();
	
private:
	std::string get_mapping_string(const input::action_map& action_map, const input::action& control);
	void add_control_item(input::action_map& action_map, input::action& control, hash::fnv1a32_t control_name_hash);
	
	std::shared_ptr<event::subscription> key_mapped_subscription;
	std::shared_ptr<event::subscription> mouse_button_mapped_subscription;
	std::shared_ptr<event::subscription> mouse_scroll_mapped_subscription;
	
	std::unique_ptr<scene::text> back_text;
	std::vector<std::unique_ptr<scene::text>> control_item_texts;
	
	bool action_remapped;
};

#endif // ANTKEEPER_KEYBOARD_CONFIG_MENU_STATE_HPP

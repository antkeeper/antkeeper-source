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

#ifndef ANTKEEPER_GAME_STATE_GAMEPAD_CONFIG_MENU_HPP
#define ANTKEEPER_GAME_STATE_GAMEPAD_CONFIG_MENU_HPP

#include "game/state/base.hpp"
#include <engine/input/action.hpp>
#include <engine/input/action-map.hpp>
#include <engine/event/subscription.hpp>
#include <cstdint>
#include <memory>

namespace state {

class gamepad_config_menu: public ::state::base
{
public:
	gamepad_config_menu(::context& ctx);
	virtual ~gamepad_config_menu();
	
private:
	std::string get_mapping_string(const input::action_map& action_map, const input::action& control);
	void add_control_item(input::action_map& action_map, input::action& control, std::uint32_t control_name_hash);
	
	std::shared_ptr<event::subscription> gamepad_axis_mapped_subscription;
	std::shared_ptr<event::subscription> gamepad_button_mapped_subscription;
	std::shared_ptr<event::subscription> key_mapped_subscription;
	
	bool action_remapped;
};

} // namespace state

#endif // ANTKEEPER_GAME_STATE_GAMEPAD_CONFIG_MENU_HPP

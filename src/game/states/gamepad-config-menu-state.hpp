// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAMEPAD_CONFIG_MENU_STATE_HPP
#define ANTKEEPER_GAMEPAD_CONFIG_MENU_STATE_HPP

#include "game/states/game-state.hpp"
#include <engine/input/action.hpp>
#include <engine/input/action-map.hpp>
#include <engine/event/subscription.hpp>
#include <engine/hash/fnv1a.hpp>
#include <cstdint>
#include <engine/scene/text.hpp>
#include <memory>


class gamepad_config_menu_state: public game_state
{
public:
	explicit gamepad_config_menu_state(::game& ctx);
	virtual ~gamepad_config_menu_state();
	
private:
	std::string get_mapping_string(const input::action_map& action_map, const input::action& control);
	void add_control_item(input::action_map& action_map, input::action& control, std::string_view control_name);
	
	std::shared_ptr<event::subscription> gamepad_axis_mapped_subscription;
	std::shared_ptr<event::subscription> gamepad_button_mapped_subscription;
	std::shared_ptr<event::subscription> key_mapped_subscription;
	
	std::unique_ptr<scene::text> back_text;
	std::vector<std::unique_ptr<scene::text>> control_item_texts;
	
	bool action_remapped;
};


#endif // ANTKEEPER_GAMEPAD_CONFIG_MENU_STATE_HPP

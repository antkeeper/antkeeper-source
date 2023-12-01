// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

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
	explicit keyboard_config_menu_state(::game& ctx);
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

// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_CONTROLS_MENU_STATE_HPP
#define ANTKEEPER_CONTROLS_MENU_STATE_HPP

#include "game/states/game-state.hpp"
#include <engine/scene/text.hpp>
#include <memory>

class controls_menu_state: public game_state
{
public:
	explicit controls_menu_state(::game& ctx);
	virtual ~controls_menu_state();
	
private:
	std::unique_ptr<scene::text> keyboard_text;
	std::unique_ptr<scene::text> gamepad_text;
	std::unique_ptr<scene::text> back_text;
};

#endif // ANTKEEPER_CONTROLS_MENU_STATE_HPP

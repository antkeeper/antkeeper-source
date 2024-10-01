// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_PAUSE_MENU_STATE_HPP
#define ANTKEEPER_PAUSE_MENU_STATE_HPP

#include "game/states/game-state.hpp"
#include <engine/scene/text.hpp>
#include <memory>

class pause_menu_state: public game_state
{
public:
	explicit pause_menu_state(::game& ctx);
	virtual ~pause_menu_state();
	
private:
	std::unique_ptr<scene::text> resume_text;
	std::unique_ptr<scene::text> options_text;
	std::unique_ptr<scene::text> main_menu_text;
	std::unique_ptr<scene::text> quit_text;
};

#endif // ANTKEEPER_PAUSE_MENU_STATE_HPP

// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_OPTIONS_MENU_STATE_HPP
#define ANTKEEPER_OPTIONS_MENU_STATE_HPP

#include "game/states/game-state.hpp"
#include <engine/scene/text.hpp>
#include <memory>

class options_menu_state: public game_state
{
public:
	explicit options_menu_state(::game& ctx);
	virtual ~options_menu_state();
	
private:
	std::unique_ptr<scene::text> controls_text;
	std::unique_ptr<scene::text> graphics_text;
	std::unique_ptr<scene::text> sound_text;
	std::unique_ptr<scene::text> language_text;
	std::unique_ptr<scene::text> back_text;
};

#endif // ANTKEEPER_OPTIONS_MENU_STATE_HPP

// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_EXTRAS_MENU_STATE_HPP
#define ANTKEEPER_EXTRAS_MENU_STATE_HPP

#include "game/states/game-state.hpp"
#include <engine/scene/text.hpp>
#include <memory>

class extras_menu_state: public game_state
{
public:
	explicit extras_menu_state(::game& ctx);
	virtual ~extras_menu_state();
	
private:
	std::unique_ptr<scene::text> credits_text;
	std::unique_ptr<scene::text> back_text;
};

#endif // ANTKEEPER_EXTRAS_MENU_STATE_HPP

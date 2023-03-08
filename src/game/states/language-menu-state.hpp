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

#ifndef ANTKEEPER_LANGUAGE_MENU_STATE_HPP
#define ANTKEEPER_LANGUAGE_MENU_STATE_HPP

#include "game/states/game-state.hpp"
#include <engine/scene/text.hpp>
#include <memory>
#include <string>
#include <vector>


class language_menu_state: public game_state
{
public:
	explicit language_menu_state(::game& ctx);
	virtual ~language_menu_state();
	
private:
	void update_text_content();
	
	std::vector<std::string> language_tags;
	std::size_t language_index;
	
	std::unique_ptr<scene::text> language_name_text;
	std::unique_ptr<scene::text> language_value_text;
	std::unique_ptr<scene::text> back_text;
};

#endif // ANTKEEPER_LANGUAGE_MENU_STATE_HPP

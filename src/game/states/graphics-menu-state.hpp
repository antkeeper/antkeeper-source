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

#ifndef ANTKEEPER_GRAPHICS_MENU_STATE_HPP
#define ANTKEEPER_GRAPHICS_MENU_STATE_HPP

#include "game/states/game-state.hpp"
#include <engine/scene/text.hpp>
#include <memory>

class graphics_menu_state: public game_state
{
public:
	explicit graphics_menu_state(::game& ctx);
	virtual ~graphics_menu_state();
	
private:
	void update_value_text_content();
	
	std::unique_ptr<scene::text> fullscreen_name_text;
	std::unique_ptr<scene::text> fullscreen_value_text;
	std::unique_ptr<scene::text> resolution_name_text;
	std::unique_ptr<scene::text> resolution_value_text;
	std::unique_ptr<scene::text> v_sync_name_text;
	std::unique_ptr<scene::text> v_sync_value_text;
	std::unique_ptr<scene::text> aa_method_name_text;
	std::unique_ptr<scene::text> aa_method_value_text;
	std::unique_ptr<scene::text> font_scale_name_text;
	std::unique_ptr<scene::text> font_scale_value_text;
	std::unique_ptr<scene::text> dyslexia_font_name_text;
	std::unique_ptr<scene::text> dyslexia_font_value_text;
	std::unique_ptr<scene::text> back_text;
};

#endif // ANTKEEPER_GRAPHICS_MENU_STATE_HPP

// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

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
	//std::unique_ptr<scene::text> aa_method_name_text;
	//std::unique_ptr<scene::text> aa_method_value_text;
	std::unique_ptr<scene::text> font_scale_name_text;
	std::unique_ptr<scene::text> font_scale_value_text;
	std::unique_ptr<scene::text> dyslexia_font_name_text;
	std::unique_ptr<scene::text> dyslexia_font_value_text;
	std::unique_ptr<scene::text> back_text;
};

#endif // ANTKEEPER_GRAPHICS_MENU_STATE_HPP

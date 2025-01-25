// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_SOUND_MENU_STATE_HPP
#define ANTKEEPER_SOUND_MENU_STATE_HPP

#include "game/states/game-state.hpp"
#include <engine/scene/text.hpp>
#include <memory>

class sound_menu_state: public game_state
{
public:
	explicit sound_menu_state(::game& ctx);
	virtual ~sound_menu_state();
	
private:
	void update_value_text_content();
	
	std::unique_ptr<scene::text> master_volume_name_text;
	std::unique_ptr<scene::text> master_volume_value_text;
	std::unique_ptr<scene::text> ambience_volume_name_text;
	std::unique_ptr<scene::text> ambience_volume_value_text;
	std::unique_ptr<scene::text> effects_volume_name_text;
	std::unique_ptr<scene::text> effects_volume_value_text;
	//std::unique_ptr<scene::text> captions_name_text;
	//std::unique_ptr<scene::text> captions_value_text;
	//std::unique_ptr<scene::text> captions_size_name_text;
	//std::unique_ptr<scene::text> captions_size_value_text;
	std::unique_ptr<scene::text> back_text;
};

#endif // ANTKEEPER_SOUND_MENU_STATE_HPP

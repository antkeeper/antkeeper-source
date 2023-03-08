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
	std::unique_ptr<scene::text> mono_audio_name_text;
	std::unique_ptr<scene::text> mono_audio_value_text;
	std::unique_ptr<scene::text> captions_name_text;
	std::unique_ptr<scene::text> captions_value_text;
	std::unique_ptr<scene::text> captions_size_name_text;
	std::unique_ptr<scene::text> captions_size_value_text;
	std::unique_ptr<scene::text> back_text;
};

#endif // ANTKEEPER_SOUND_MENU_STATE_HPP

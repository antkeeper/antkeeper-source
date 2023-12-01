// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_LANGUAGE_MENU_STATE_HPP
#define ANTKEEPER_LANGUAGE_MENU_STATE_HPP

#include "game/states/game-state.hpp"
#include <engine/scene/text.hpp>
#include <engine/utility/text-file.hpp>
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
	
	std::shared_ptr<text_file> language_manifest;
	std::size_t language_index;
	
	std::unique_ptr<scene::text> language_name_text;
	std::unique_ptr<scene::text> language_value_text;
	std::unique_ptr<scene::text> back_text;
};

#endif // ANTKEEPER_LANGUAGE_MENU_STATE_HPP

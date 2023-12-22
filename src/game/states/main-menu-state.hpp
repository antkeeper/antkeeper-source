// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_MAIN_MENU_STATE_HPP
#define ANTKEEPER_MAIN_MENU_STATE_HPP

#include "game/states/game-state.hpp"
#include <engine/scene/text.hpp>
#include <engine/animation/animation.hpp>
#include <engine/entity/id.hpp>
#include <engine/event/subscription.hpp>
#include <memory>


class main_menu_state: public game_state
{
public:
	explicit main_menu_state(::game& ctx, bool fade_in);
	virtual ~main_menu_state();
	
private:
	void fade_in_title();
	void fade_out_title();
	
	std::unique_ptr<scene::text> title_text;
	std::unique_ptr<scene::text> start_text;
	std::unique_ptr<scene::text> options_text;
	std::unique_ptr<scene::text> extras_text;
	std::unique_ptr<scene::text> quit_text;
	
	animation<float> title_fade_animation;
	
	std::shared_ptr<event::subscription> window_resized_subscription;
};

#endif // ANTKEEPER_MAIN_MENU_STATE_HPP

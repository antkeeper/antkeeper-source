// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_CREDITS_STATE_HPP
#define ANTKEEPER_CREDITS_STATE_HPP

#include "game/states/game-state.hpp"
#include <engine/scene/text.hpp>
#include <engine/animation/animation-sequence.hpp>
#include <engine/event/subscription.hpp>
#include <engine/entity/id.hpp>
#include <vector>

class credits_state: public game_state
{
public:
	explicit credits_state(::game& ctx);
	virtual ~credits_state();
	
private:
	scene::text credits_text;
	entity::id m_credits_entity{entt::null};
	std::shared_ptr<animation_sequence> m_roll_credits_sequence;

	std::vector<std::shared_ptr<::event::subscription>> input_mapped_subscriptions;
	std::shared_ptr<event::subscription> window_resized_subscription;
};

#endif // ANTKEEPER_CREDITS_STATE_HPP

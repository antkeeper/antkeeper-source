// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_STEERING_SYSTEM_HPP
#define ANTKEEPER_GAME_STEERING_SYSTEM_HPP

#include "game/systems/updatable-system.hpp"


class steering_system:
	public updatable_system
{
public:
	explicit steering_system(entity::registry& registry);
	~steering_system() override = default;
	
	virtual void update(float t, float dt);
};


#endif // ANTKEEPER_GAME_STEERING_SYSTEM_HPP

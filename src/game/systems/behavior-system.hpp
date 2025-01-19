// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_BEHAVIOR_SYSTEM_HPP
#define ANTKEEPER_GAME_BEHAVIOR_SYSTEM_HPP

#include "game/systems/updatable-system.hpp"

class behavior_system:
	public updatable_system
{
public:
	explicit behavior_system(entity::registry& registry);
	~behavior_system() override = default;
	virtual void update(float t, float dt);
};


#endif // ANTKEEPER_GAME_BEHAVIOR_SYSTEM_HPP

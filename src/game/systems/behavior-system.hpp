// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_BEHAVIOR_SYSTEM_HPP
#define ANTKEEPER_GAME_BEHAVIOR_SYSTEM_HPP

#include "game/systems/fixed-update-system.hpp"

using namespace engine;

class behavior_system:
	public fixed_update_system
{
public:
	~behavior_system() override = default;
	void fixed_update(entity::registry& registry, float t, float dt) override;
};

#endif // ANTKEEPER_GAME_BEHAVIOR_SYSTEM_HPP

// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_LOCOMOTION_SYSTEM_HPP
#define ANTKEEPER_GAME_LOCOMOTION_SYSTEM_HPP

#include "game/systems/fixed-update-system.hpp"

class locomotion_system:
	public fixed_update_system
{
public:
	~locomotion_system() override = default;
	void fixed_update(entity::registry& registry, float t, float dt) override;
	
private:
	void update_legged(entity::registry& registry, float t, float dt);
	void update_winged(entity::registry& registry, float t, float dt);
};

#endif // ANTKEEPER_GAME_LOCOMOTION_SYSTEM_HPP

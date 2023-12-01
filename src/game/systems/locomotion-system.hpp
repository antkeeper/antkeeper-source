// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_LOCOMOTION_SYSTEM_HPP
#define ANTKEEPER_GAME_LOCOMOTION_SYSTEM_HPP

#include "game/systems/updatable-system.hpp"

/**
 * 
 */
class locomotion_system:
	public updatable_system
{
public:
	explicit locomotion_system(entity::registry& registry);
	void update(float t, float dt) override;
	
private:
	void update_legged(float t, float dt);
	void update_winged(float t, float dt);
};

#endif // ANTKEEPER_GAME_LOCOMOTION_SYSTEM_HPP

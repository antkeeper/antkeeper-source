// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_IK_SYSTEM_HPP
#define ANTKEEPER_GAME_IK_SYSTEM_HPP

#include "game/systems/updatable-system.hpp"

class ik_system:
	public updatable_system
{
public:
	explicit ik_system(entity::registry& registry);
	~ik_system() override = default;
	void update(float t, float dt) override;
};

#endif // ANTKEEPER_GAME_IK_SYSTEM_HPP

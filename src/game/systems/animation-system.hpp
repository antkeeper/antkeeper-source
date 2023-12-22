// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_ANIMATION_SYSTEM_HPP
#define ANTKEEPER_GAME_ANIMATION_SYSTEM_HPP

#include "game/systems/updatable-system.hpp"

/**
 * 
 */
class animation_system:
	public updatable_system
{
public:
	explicit animation_system(entity::registry& registry);
	~animation_system() override = default;
	void update(float t, float dt) override;
	void interpolate(float alpha);
};

#endif // ANTKEEPER_GAME_ANIMATION_SYSTEM_HPP

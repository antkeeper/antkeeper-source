// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_TERRAIN_SYSTEM_HPP
#define ANTKEEPER_GAME_TERRAIN_SYSTEM_HPP

#include "game/systems/fixed-update-system.hpp"

/// Generates terrain patches and performs terrain patch LOD selection.
class terrain_system: public fixed_update_system
{
public:
	~terrain_system() override = default;
	void fixed_update(entity::registry& registry, float t, float dt) override;
};

#endif // ANTKEEPER_GAME_TERRAIN_SYSTEM_HPP

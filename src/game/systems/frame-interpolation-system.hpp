// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_FRAME_INTERPOLATION_SYSTEM_HPP
#define ANTKEEPER_GAME_FRAME_INTERPOLATION_SYSTEM_HPP

#include "game/systems/variable-update-system.hpp"

/// Interpolates scene object transforms between fixed-rate updates.
class frame_interpolation_system:
	public variable_update_system
{
public:
	~frame_interpolation_system() override = default;
	void variable_update(entity::registry& registry, float t, float dt, float alpha) override;
};

#endif // ANTKEEPER_GAME_FRAME_INTERPOLATION_SYSTEM_HPP

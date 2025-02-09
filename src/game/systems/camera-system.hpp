// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_CAMERA_SYSTEM_HPP
#define ANTKEEPER_GAME_CAMERA_SYSTEM_HPP

#include "game/systems/fixed-update-system.hpp"
#include "game/systems/variable-update-system.hpp"

class camera_system:
	public fixed_update_system,
	public variable_update_system
{
public:
	~camera_system() override = default;
	void fixed_update(entity::registry& registry, float t, float dt) override;
	void variable_update(entity::registry& registry, float t, float dt, float alpha) override;

private:
	double m_variable_update_time{};
};

#endif // ANTKEEPER_GAME_CAMERA_SYSTEM_HPP

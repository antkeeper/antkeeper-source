// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_STEERING_SYSTEM_HPP
#define ANTKEEPER_GAME_STEERING_SYSTEM_HPP

#include "game/systems/fixed-update-system.hpp"
#include <engine/math/vector.hpp>

using namespace engine;

class steering_system:
	public fixed_update_system
{
public:
	~steering_system() override = default;
	void fixed_update(entity::registry& registry, float t, float dt) override;

	static inline constexpr math::fvec3 global_forward{0.0f, 0.0f, -1.0f};
	static inline constexpr math::fvec3 global_up{0.0f, 1.0f, 0.0f};
	static inline constexpr math::fvec3 global_right{1.0f, 0.0f, 0.0f};
};

#endif // ANTKEEPER_GAME_STEERING_SYSTEM_HPP

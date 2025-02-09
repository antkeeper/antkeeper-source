// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_STEERING_SYSTEM_HPP
#define ANTKEEPER_GAME_STEERING_SYSTEM_HPP

#include "game/systems/updatable-system.hpp"
#include <engine/math/vector.hpp>

class steering_system:
	public updatable_system
{
public:
	explicit steering_system(entity::registry& registry);
	~steering_system() override = default;
	
	void update(float t, float dt) override;

	static inline constexpr math::fvec3 global_forward{0.0f, 0.0f, -1.0f};
	static inline constexpr math::fvec3 global_up{0.0f, 1.0f, 0.0f};
	static inline constexpr math::fvec3 global_right{1.0f, 0.0f, 0.0f};
};

#endif // ANTKEEPER_GAME_STEERING_SYSTEM_HPP

// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_FIXED_UPDATE_SYSTEM_HPP
#define ANTKEEPER_GAME_FIXED_UPDATE_SYSTEM_HPP

#include "game/systems/component-system.hpp"
#include <engine/entity/registry.hpp>

/// Abstract base class for systems which perform fixed-rate updates.
class fixed_update_system: public virtual component_system
{
public:
	/// Destructs the fixed update system.
	virtual ~fixed_update_system() = default;

	/// Performs a fixed-rate update.
	/// @param registry Component registry.
	/// @param t Elapsed time, in seconds.
	/// @param dt Fixed-rate update interval, in seconds.
	virtual void fixed_update(entity::registry& registry, float t, float dt) /*const*/ = 0;
};

#endif // ANTKEEPER_GAME_FIXED_UPDATE_SYSTEM_HPP

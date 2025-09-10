// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_VARIABLE_UPDATE_SYSTEM_HPP
#define ANTKEEPER_GAME_VARIABLE_UPDATE_SYSTEM_HPP

#include "game/systems/component-system.hpp"
#include <engine/entity/registry.hpp>

using namespace engine;

/// Abstract base class for systems which perform variable-rate updates.
class variable_update_system: public virtual component_system
{
public:
	/// Destructs the variable update system.
	virtual ~variable_update_system() = default;

	/// Performs a variable-rate update.
	/// @param registry Component registry.
	/// @param t Elapsed time, in seconds.
	/// @param dt Fixed-rate update interval, in seconds.
	/// @param alpha Subframe interpolation factor.
	virtual void variable_update(entity::registry& registry, float t, float dt, float alpha) /*const*/ = 0;
};

#endif // ANTKEEPER_GAME_VARIABLE_UPDATE_SYSTEM_HPP

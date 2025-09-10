// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_UTILITY_TIME_HPP
#define ANTKEEPER_GAME_UTILITY_TIME_HPP

#include <engine/entity/registry.hpp>

using namespace engine;

/// Sets the time scale in the time singleton component. If no time component was found, it will be created.
/// @param registry Entity registry.
/// @param time_scale Time scale.
void set_time_scale(entity::registry& registry, float time_scale);

/// Returns the time scale from the time singleton component. If no time component was found, returns 1.0f.
/// @param registry Entity registry.
[[nodiscard]] float get_time_scale(const entity::registry& registry);

#endif // ANTKEEPER_GAME_UTILITY_TIME_HPP

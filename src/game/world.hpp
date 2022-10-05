/*
 * Copyright (C) 2021  Christopher J. Howard
 *
 * This file is part of Antkeeper source code.
 *
 * Antkeeper source code is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Antkeeper source code is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Antkeeper source code.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef ANTKEEPER_GAME_WORLD_HPP
#define ANTKEEPER_GAME_WORLD_HPP

#include "game/context.hpp"

namespace game {

/// World creation and manipulation functions.
namespace world {

/// Creates the cosmos.
void cosmogenesis(game::context& ctx);

/// Creates the observer.
void create_observer(game::context& ctx);

/**
 * Sets the location of the observer.
 *
 * @param ctx Game context.
 * @param elevation Elevation, in meters.
 * @param latitude Latitude, in radians.
 * @param longitude Longitude, in radians.
 */
void set_location(game::context& ctx, double elevation, double latitude, double longitude);

/**
 * Sets the current time.
 *
 * @param ctx Game context.
 * @param t UT1 time, in days.
 */
void set_time(game::context& ctx, double t);

/**
 * Sets the current time.
 *
 * @param ctx Game context.
 * @param year Astronomical year numbering. 1 BC is `0`, 2 BC is `-1`.
 * @param month Month number on `[1, 12]`.
 * @param day Day number on `[1, 31]`.
 * @param hour Hour number on `[0, 23]`.
 * @param minute Minute number on `[0, 59]`.
 * @param second Fractional second on `[0.0, 60.0)`.
 */
void set_time(game::context& ctx, int year, int month, int day, int hour, int minute, double second);

/**
 * Sets rate at which time passes.
 *
 * @param ctx Game context.
 * @param scale Time scale.
 */
void set_time_scale(game::context& ctx, double scale);

} // namespace menu
} // namespace game

#endif // ANTKEEPER_GAME_WORLD_HPP

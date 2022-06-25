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

/// Creates the fixed stars.
void create_stars(game::context& ctx);

/// Creates the sun.
void create_sun(game::context& ctx);

/// Creates the planet.
void create_planet(game::context& ctx);

/// Creates the moon.
void create_moon(game::context& ctx);

/// Sets the current time.
void set_time(game::context& ctx, double t);

/// Sets rate at which time passes.
void set_time_scale(game::context& ctx, double scale);

} // namespace menu
} // namespace game

#endif // ANTKEEPER_GAME_WORLD_HPP

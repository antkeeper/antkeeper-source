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

#ifndef ANTKEEPER_GAME_ANT_TRAIT_FORAGING_TIME_HPP
#define ANTKEEPER_GAME_ANT_TRAIT_FORAGING_TIME_HPP

#include "render/model.hpp"

namespace game {
namespace ant {
namespace trait {

/**
 * Ant foraging time description.
 */
struct foraging_time
{
	/// Minimum solar altitude at which foraging occurs.
	float solar_altitude_min;
	
	/// Maximum solar alttiude at which foraging occurs.
	float solar_altitude_max;
};

} // namespace trait
} // namespace ant
} // namespace game

#endif // ANTKEEPER_GAME_ANT_TRAIT_FORAGING_TIME_HPP

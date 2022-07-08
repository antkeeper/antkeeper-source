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

#ifndef ANTKEEPER_GAME_ANT_TRAIT_DIET_HPP
#define ANTKEEPER_GAME_ANT_TRAIT_DIET_HPP

namespace game {
namespace ant {
namespace trait {

/**
 * Trait that describes the diet of an ant.
 */
struct diet
{
	/// Preference for eating seeds.
	float seeds;
	
	/// Preference for eating ant brood.
	float ant_brood;
	
	/// Preference for eating arthropod eggs.
	float arthropod_eggs;
	
	/// Preference for eating nectar.
	float nectar;
	
	/// Preference for eating fungi.
	float fungi;
	
	/// Preference for eating carrion.
	float carrion;
};

} // namespace trait
} // namespace ant
} // namespace game

#endif // ANTKEEPER_GAME_ANT_TRAIT_DIET_HPP

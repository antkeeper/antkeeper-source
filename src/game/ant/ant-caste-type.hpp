/*
 * Copyright (C) 2023  Christopher J. Howard
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

#ifndef ANTKEEPER_GAME_ANT_CASTE_TYPE_HPP
#define ANTKEEPER_GAME_ANT_CASTE_TYPE_HPP

#include <cstdint>

/**
 * Ant caste types.
 *
 * @see https://www.antwiki.org/wiki/Caste_Terminology
 */
enum class ant_caste_type: std::uint8_t
{
	/// Queen caste.
	queen   = 0b0001,
	
	/// Worker caste.
	worker  = 0b0010,
	
	/// Soldier caste.
	soldier = 0b0100,
	
	/// Male caste.
	male    = 0b1000
};

#endif // ANTKEEPER_GAME_ANT_CASTE_TYPE_HPP

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

#ifndef ANTKEEPER_GAME_ANT_SUBCASTE_HPP
#define ANTKEEPER_GAME_ANT_SUBCASTE_HPP

#include <cstdint>

/**
 * Ant subcaste types.
 */
enum class ant_subcaste: std::uint8_t
{
	/// Worker from the queen's first batch of eggs, smaller than normal workers.
	nanitic = 1,
	
	/// Small worker or soldier.
	minor,
	
	/// Normal-sized worker or soldier
	media,
	
	/// Large worker or soldier.
	major,
	
	/// Winged queen or male.
	alate,
	
	/// Dewinged queen or male.
	dealate,
	
	/// Queen or male which does not develop wings.
	ergatoid,
	
	/// Queen or male with short, nonfunctional wings.
	brachypterous
};

#endif // ANTKEEPER_GAME_ANT_SUBCASTE_HPP

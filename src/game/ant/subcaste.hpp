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

#ifndef ANTKEEPER_GAME_ANT_SUBCASTE_HPP
#define ANTKEEPER_GAME_ANT_SUBCASTE_HPP

namespace game {
namespace ant {

/**
 * Ant subcaste types.
 */
enum class subcaste
{
	/// A worker from the queen's first batch of eggs, smaller than normal workers.
	nanitic,
	
	/// A small worker or soldier.
	minor,
	
	/// A normal-sized worker or soldier
	media,
	
	/// A large worker or soldier.
	major,
	
	/// A queen or male which still has its wings.
	alate,
	
	/// A queen or male which has shed its wings.
	dealate,
	
	/// A queen or male which does not develop wings.
	ergatoid,
	
	/// A queen or male with short, nonfunctional wings.
	brachypterous
};

} // namespace ant
} // namespace game

#endif // ANTKEEPER_GAME_ANT_SUBCASTE_HPP

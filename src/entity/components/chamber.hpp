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

#ifndef ANTKEEPER_ENTITY_COMPONENT_CHAMBER_HPP
#define ANTKEEPER_ENTITY_COMPONENT_CHAMBER_HPP

#include <array>
#include <unordered_set>
#include "entity/id.hpp"

namespace entity {
namespace component {

/// Ant nest chamber.
struct chamber
{
	/// Entity ID of shaft to which the chamber is connected
	entity::id shaft_eid;
	
	/// Distance along shaft at which the chamber is located
	float distance;
	
	/// Entity ID of the chamber above this chamber
	entity::id previous_chamber_eid;
	
	/// Entity ID of the chamber below this chamber
	entity::id next_chamber_eid;
	
	float outer_radius;
	float inner_radius;
	float inner_sector_angle;
	float tile_radius;
	//std::unordered_set<std::array<int, 2>> tiles;
};

} // namespace component
} // namespace entity

#endif // ANTKEEPER_ENTITY_COMPONENT_CHAMBER_HPP

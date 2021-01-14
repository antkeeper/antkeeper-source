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

#ifndef ANTKEEPER_ECS_CHAMBER_COMPONENT_HPP
#define ANTKEEPER_ECS_CHAMBER_COMPONENT_HPP

#include <array>
#include <unordered_set>
#include "ecs/entity.hpp"

namespace ecs {

struct chamber_component
{
	entity nest;
	float depth;
	float outer_radius;
	float inner_radius;
	float inner_sector_angle;
	float tile_radius;
	std::unordered_set<std::array<int, 2>> tiles;
}

} // namespace ecs

#endif // ANTKEEPER_ECS_CHAMBER_COMPONENT_HPP

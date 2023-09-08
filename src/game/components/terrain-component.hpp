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

#ifndef ANTKEEPER_GAME_TERRAIN_COMPONENT_HPP
#define ANTKEEPER_GAME_TERRAIN_COMPONENT_HPP

#include <engine/entity/id.hpp>
#include <engine/math/vector.hpp>
#include <vector>

/// Grid of terrain cells.
struct terrain_grid_component
{
	math::uvec2 dimensions;
	std::vector<entity::id> cells;
};

/// Single cell in a terrain grid.
struct terrain_cell_component
{
	entity::id grid_eid;
	math::uvec2 coordinates;
};


#endif // ANTKEEPER_GAME_TERRAIN_COMPONENT_HPP

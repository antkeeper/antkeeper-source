// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

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

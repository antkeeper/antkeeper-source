// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_CHAMBER_COMPONENT_HPP
#define ANTKEEPER_GAME_CHAMBER_COMPONENT_HPP

#include <engine/entity/id.hpp>
#include <array>
#include <unordered_set>

using namespace engine;

/// Ant nest chamber.
struct chamber_component
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

#endif // ANTKEEPER_GAME_CHAMBER_COMPONENT_HPP

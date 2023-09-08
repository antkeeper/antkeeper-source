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

#ifndef ANTKEEPER_GAME_NAVMESH_AGENT_COMPONENT_HPP
#define ANTKEEPER_GAME_NAVMESH_AGENT_COMPONENT_HPP

#include <engine/entity/id.hpp>
#include <engine/math/vector.hpp>
#include <engine/geom/brep/brep-mesh.hpp>

/**
 * 
 */
struct navmesh_agent_component
{
	/// Entity ID of the navmesh
	entity::id navmesh_eid{entt::null};
	
	/// Pointer to the current mesh through which the agent is navigating.
	geom::brep_mesh* mesh{};
	
	/// Pointer to the current mesh face on which the agent is located.
	geom::brep_face* face{};
	
	/// Smooth interpolated surface normal at the agent position.
	math::fvec3 surface_normal{};
};

#endif // ANTKEEPER_GAME_NAVMESH_AGENT_COMPONENT_HPP

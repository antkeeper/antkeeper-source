// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

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

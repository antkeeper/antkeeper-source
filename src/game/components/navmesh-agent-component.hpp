// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_NAVMESH_AGENT_COMPONENT_HPP
#define ANTKEEPER_GAME_NAVMESH_AGENT_COMPONENT_HPP

import engine.geom.brep.mesh;
import engine.geom.brep.feature;
import engine.entity.id;
import engine.math.vector;

using namespace engine;

struct navmesh_agent_component
{
	/// Entity ID of the navmesh
	entity::id navmesh_eid{entt::null};
	
	/// Pointer to the current mesh through which the agent is navigating.
	geom::brep::mesh* mesh{};
	
	/// Current mesh feature on which the agent is located.
	geom::brep::feature feature{};
	
	/// Smooth interpolated surface normal at the agent position.
	math::fvec3 surface_normal{};
};

#endif // ANTKEEPER_GAME_NAVMESH_AGENT_COMPONENT_HPP

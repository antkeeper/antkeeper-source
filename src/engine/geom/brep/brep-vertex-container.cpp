// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <engine/geom/brep/brep-vertex-container.hpp>
#include <engine/geom/brep/brep-mesh.hpp>

namespace geom {

brep_vertex* brep_vertex_container::emplace_back()
{
	brep_vertex* vertex = brep_element_container<brep_vertex>::emplace_back();
	vertex->m_index = size() - 1;
	vertex->m_edges.m_vertex = vertex;
	return vertex;
};

void brep_vertex_container::erase(brep_vertex* vertex)
{
	// Erase all edges bounded by this vertex
	while (!vertex->edges().empty())
	{
		m_mesh->edges().erase(vertex->edges().back());
	}
	
	// Erase vertex
	brep_element_container<brep_vertex>::erase(vertex);
}

void brep_vertex_container::clear() noexcept
{
	while (!empty())
	{
		erase(back());
	}
}

} // namespace geom

// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <engine/geom/brep/vertex-container.hpp>
#include <engine/geom/brep/mesh.hpp>

namespace engine::geom::brep
{
	vertex* vertex_container::emplace_back()
	{
		auto vertex = element_container<brep::vertex>::emplace_back();
		vertex->m_index = size() - 1;
		vertex->m_edges.m_vertex = vertex;
		return vertex;
	};

	void vertex_container::erase(brep::vertex* vertex)
	{
		// Erase all edges bounded by this vertex
		while (!vertex->edges().empty())
		{
			m_mesh->edges().erase(vertex->edges().back());
		}

		// Erase vertex
		element_container<brep::vertex>::erase(vertex);
	}

	void vertex_container::clear() noexcept
	{
		while (!empty())
		{
			erase(back());
		}
	}
}

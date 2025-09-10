// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <engine/geom/brep/edge-container.hpp>
#include <engine/geom/brep/mesh.hpp>
#include <engine/utility/sized-types.hpp>
#include <engine/math/functions.hpp>

namespace engine::geom::brep
{
	edge* edge_container::emplace_back(vertex* a, vertex* b)
	{
		if (a == b)
		{
			return nullptr;
		}

		auto ab = element_container<edge>::emplace_back();
		ab->m_index = size() - 1;
		ab->m_vertices[0] = a;
		ab->m_vertices[1] = b;

		// Append edge AB to the edge lists of vertices A and B
		a->m_edges.push_back(ab);
		b->m_edges.push_back(ab);

		return ab;
	};

	void edge_container::erase(brep::edge* edge)
	{
		// Kill all loops and faces bounded by this edge
		while (!edge->loops().empty())
		{
			m_mesh->faces().erase(edge->loops().back()->face());
		}

		// Remove this edge from its vertices' lists of edges
		edge->vertices().front()->m_edges.remove(edge);
		edge->vertices().back()->m_edges.remove(edge);

		// Erase edge
		element_container<brep::edge>::erase(edge);
	}

	void edge_container::clear() noexcept
	{
		while (!empty())
		{
			erase(back());
		}
	}

	edge* edge_container::find(vertex* a, vertex* b) const
	{
		if (!a->edges().empty() && !b->edges().empty() && a != b)
		{
			edge* ea = a->edges().front();
			edge* eb = b->edges().front();
			const usize n = math::min(a->edges().size(), b->edges().size());
			for (usize i = 0; i < n; ++i)
			{
				if (ea->vertices()[1] == b || ea->vertices()[0] == b)
				{
					return ea;
				}

				if (eb->vertices()[1] == a || eb->vertices()[0] == a)
				{
					return eb;
				}

				ea = ea->m_vertex_next[ea->vertices()[1] == a];
				eb = eb->m_vertex_next[eb->vertices()[1] == b];
			}
		}

		return nullptr;
	}
}

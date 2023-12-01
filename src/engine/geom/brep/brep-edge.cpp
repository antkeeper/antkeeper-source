// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <engine/geom/brep/brep-edge.hpp>
#include <engine/geom/brep/brep-mesh.hpp>
#include <algorithm>

namespace geom {

void brep_edge_loop_list::push_back(brep_loop* loop)
{
	if (empty())
	{
		// List empty, initialize
		m_head = loop;
		loop->m_edge_next = loop;
		loop->m_edge_previous = loop;
	}
	else
	{
		// Append loop
		loop->m_edge_next = m_head;
		loop->m_edge_previous = m_head->m_edge_previous;
		m_head->m_edge_previous->m_edge_next = loop;
		m_head->m_edge_previous = loop;
	}
	
	++m_size;
}

void brep_edge_loop_list::remove(brep_loop* loop)
{
	// Directly link next and previous loops
	loop->m_edge_next->m_edge_previous = loop->m_edge_previous;
	loop->m_edge_previous->m_edge_next = loop->m_edge_next;
	
	// If loop was the list head, update head
	if (m_head == loop)
	{
		m_head = loop->m_edge_next;
	}
	
	--m_size;
}

brep_edge* brep_edge_container::emplace_back(brep_vertex* a, brep_vertex* b)
{
	if (a == b)
	{
		return nullptr;
	}
	
	brep_edge* ab = brep_element_container<brep_edge>::emplace_back();
	ab->m_index = size() - 1;
	ab->m_vertices[0] = a;
	ab->m_vertices[1] = b;
	
	// Append edge AB to the edge lists of vertices A and B
	a->m_edges.push_back(ab);
	b->m_edges.push_back(ab);
	
	return ab;
};

void brep_edge_container::erase(brep_edge* edge)
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
	brep_element_container<brep_edge>::erase(edge);
}

void brep_edge_container::clear() noexcept
{
	while (!empty())
	{
		erase(back());
	}
}

brep_edge* brep_edge_container::find(brep_vertex* a, brep_vertex* b) const
{
	if (!a->edges().empty() && !b->edges().empty() && a != b)
	{
		brep_edge* ea = a->edges().front();
		brep_edge* eb = b->edges().front();
		const std::size_t n = std::min<std::size_t>(a->edges().size(), b->edges().size());
		for (std::size_t i = 0; i < n; ++i)
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

} // namespace geom

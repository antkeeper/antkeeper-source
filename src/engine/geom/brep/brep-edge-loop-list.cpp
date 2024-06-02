// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <engine/geom/brep/brep-edge-loop-list.hpp>

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

} // namespace geom

// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <engine/geom/brep/brep-face.hpp>
#include <engine/geom/brep/brep-mesh.hpp>
#include <algorithm>
#include <vector>

namespace geom {

void brep_face_loop_list::push_back(brep_loop* loop)
{
	if (empty())
	{
		// List empty, initialize
		m_head = loop;
		loop->m_face_next = loop;
		loop->m_face_previous = loop;
	}
	else
	{
		// Append loop
		loop->m_face_next = m_head;
		loop->m_face_previous = m_head->m_face_previous;
		m_head->m_face_previous->m_face_next = loop;
		m_head->m_face_previous = loop;
	}
	
	++m_size;
}

void brep_face_loop_list::insert(brep_loop* next, brep_loop* loop)
{
	loop->m_face_next = next;
	loop->m_face_previous = next->m_face_previous;
	next->m_face_previous->m_face_next = loop;
	next->m_face_previous = loop;
	
	++m_size;
}

void brep_face_loop_list::remove(brep_loop* loop)
{
	// Directly link next and previous loops
	loop->m_face_next->m_face_previous = loop->m_face_previous;
	loop->m_face_previous->m_face_next = loop->m_face_next;
	
	// If loop was the list head, update head
	if (m_head == loop)
	{
		m_head = loop->m_face_next;
	}
	
	--m_size;
}

brep_face* brep_face_container::emplace_back(const std::span<brep_vertex*> vertices)
{
	if (vertices.size() < 3)
	{
		return nullptr;
	}
	
	// Find or make edges
	std::vector<brep_edge*> edges(vertices.size());
	std::size_t i = vertices.size() - 1;
	for (std::size_t j = 0; j < vertices.size(); ++j)
	{
		edges[i] = m_mesh->edges().find(vertices[i], vertices[j]);
		if (!edges[i])
		{
			edges[i] = m_mesh->edges().emplace_back(vertices[i], vertices[j]);
		}
		
		i = j;
	}
	
	// Allocate face
	brep_face* face = brep_element_container<brep_face>::emplace_back();
	face->m_index = size() - 1;
	
	// Make face loops
	for (std::size_t i = 0; i < vertices.size(); ++i)
	{
		brep_loop* loop = m_mesh->loops().emplace_back();
		
		loop->m_vertex = vertices[i];
		loop->m_edge = edges[i];
		loop->m_face = face;
		
		// Append loop to its edge's list of loops
		loop->m_edge->m_loops.push_back(loop);
		
		// Append loop to its face's list of loops
		face->m_loops.push_back(loop);
	}
	
	return face;
};

void brep_face_container::erase(brep_face* face)
{
	brep_loop* loop = face->loops().front();
	do
	{
		// Remove loop from its edge's list of loops
		loop->m_edge->m_loops.remove(loop);
		
		brep_loop* next_loop = loop->m_face_next;
		
		// Erase loop
		m_mesh->loops().erase(loop);
		
		loop = next_loop;
	}
	while (loop != face->loops().front());
	
	// Erase face
	brep_element_container<brep_face>::erase(face);
}

void brep_face_container::clear() noexcept
{
	while (!empty())
	{
		erase(back());
	}
}

void brep_face_container::reverse(brep_face* face)
{
	for (brep_loop* loop: face->loops())
	{
		// Swap order of loop vertices
		loop->m_vertex = loop->edge()->vertices()[loop->edge()->vertices()[0] == loop->vertex()];
		
		// Swap pointers to next and previous face loops
		std::swap(loop->m_face_next, loop->m_face_previous);
	}
}

} // namespace geom

/*
 * Copyright (C) 2021  Christopher J. Howard
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

#include "mesh.hpp"
#include <stdexcept>

mesh::~mesh()
{
	// Deallocate vertices
	for (mesh::vertex* vertex: vertices)
	{		
		delete vertex;
	}

	// Deallocate edges
	for (mesh::edge* edge: edges)
	{
		delete edge->symmetric;
		delete edge;
	}

	// Deallocate faces
	for (mesh::face* face: faces)
	{
		delete face;
	}
}

mesh::vertex* mesh::add_vertex(const float3& position)
{
	mesh::vertex* vertex = new mesh::vertex();
	vertex->edge = nullptr;
	vertex->position = position;
	vertex->index = vertices.size();
	vertices.push_back(vertex);

	return vertex;
}

mesh::edge* mesh::add_edge(mesh::vertex* a, mesh::vertex* b)
{
	mesh::edge* ab = new mesh::edge();
	mesh::edge* ba = new mesh::edge();

	ab->vertex = a;
	ab->face = nullptr;
	ab->previous = ba;
	ab->next = ba;
	ab->symmetric = ba;
	ab->index = edges.size();

	ba->vertex = b;
	ba->face = nullptr;
	ba->previous = ab;
	ba->next = ab;
	ba->symmetric = ab;
	ba->index = edges.size();

	if (!a->edge)
	{
		a->edge = ab;
	}
	else
	{
		mesh::edge* a_in = find_free_incident(a);
		mesh::edge* a_out = a_in->next;
		a_in->next = ab;
		ab->previous = a_in;
		ba->next = a_out;
		a_out->previous = ba;
	}

	if (!b->edge)
	{
		b->edge = ba;
	}
	else
	{
		mesh::edge* b_in = find_free_incident(b);
		mesh::edge* b_out = b_in->next;
		b_in->next = ba;
		ba->previous = b_in;
		ab->next = b_out;
		b_out->previous = ab;
	}

	// Add edge
	edges.push_back(ab);

	return ab;
}

mesh::face* mesh::add_face(const loop& loop)
{
	if (loop.empty())
	{
		throw std::runtime_error("Empty edge loop");
	}

	// Validate edge loop
	for (std::size_t i = 0; i < loop.size(); ++i)
	{
		mesh::edge* current = loop[i];
		mesh::edge* next = loop[(i + 1) % loop.size()];

		if (current->symmetric->vertex != next->vertex)
		{
			// Disconnected edge loop
			throw std::runtime_error("Disconnected edge loop");
		}

		if (current->face)
		{
			// This edge already has a face
			throw std::runtime_error("Non-manifold mesh 1");
		}
	}

	// Make edges adjacent
	for (std::size_t i = 0; i < loop.size(); ++i)
	{
		if (!make_adjacent(loop[i], loop[(i + 1) % loop.size()]))
		{
			throw std::runtime_error("Non-manifold mesh 2");
		}
	}

	// Create face
	mesh::face* face = new mesh::face();
	face->edge = loop[0];
	face->index = faces.size();

	// Add face
	faces.push_back(face);

	// Connect edges to the face
	for (mesh::edge* edge: loop)
	{
		edge->face = face;
	}

	return face;
}

void mesh::remove_face(mesh::face* face)
{
	// Nullify pointers to this face
	mesh::edge* edge = face->edge;
	do
	{
		edge->face = nullptr;
		edge = edge->next;
	}
	while (edge != face->edge);

	// Adjust indices of faces after this face
	for (std::size_t i = face->index + 1; i < faces.size(); ++i)
	{
		--faces[i]->index;
	}

	// Remove face from the faces vector
	faces.erase(faces.begin() + face->index);

	// Deallocate face
	delete face;
}

void mesh::remove_edge(mesh::edge* edge)
{
	mesh::edge* ab = edge;
	mesh::edge* ba = edge->symmetric;
	mesh::vertex* a = ab->vertex;
	mesh::edge* a_in = ab->previous;
	mesh::edge* a_out = ba->next;
	mesh::vertex* b = ba->vertex;
	mesh::edge* b_in = ba->previous;
	mesh::edge* b_out = ab->next;

	// Remove dependent faces
	if (ab->face)
		remove_face(ab->face);
	if (ba->face)
		remove_face(ba->face);

	// Re-link edges
	if (a->edge == ab)
		a->edge = (a_out == ab) ? nullptr : a_out;
	if (b->edge == ba)
		b->edge = (b_out == ba) ? nullptr : b_out;
	a_in->next = a_out;
	a_out->previous = a_in;
	b_in->next = b_out;
	b_out->previous = b_in;

	// Adjust indices of edges after this edge
	for (std::size_t i = edge->index + 1; i < edges.size(); ++i)
	{
		--edges[i]->index;
		--edges[i]->symmetric->index;
	}

	// Remove edge from the edges vector
	edges.erase(edges.begin() + edge->index);

	// Deallocate edge
	delete edge->symmetric;
	delete edge;
}

void mesh::remove_vertex(mesh::vertex* vertex)
{
	// Remove connected edges
	if (vertex->edge)
	{
		mesh::edge* current = nullptr;
		mesh::edge* next = vertex->edge;

		do
		{
			current = next;

			next = next->symmetric->next;
			if (next == current)
			{
				next = next->symmetric->next;
			}

			remove_edge(current);
		}
		while (current != next);
	}

	// Adjust indices of vertices after this vertex
	for (std::size_t i = vertex->index + 1; i < vertices.size(); ++i)
	{
		--vertices[i]->index;
	}

	// Remove vertex from the vertices vector
	vertices.erase(vertices.begin() + vertex->index);

	// Deallocate vertex
	delete vertex;
}

mesh::edge* mesh::find_free_incident(mesh::vertex* vertex) const
{
	mesh::edge* begin = vertex->edge->symmetric;
	mesh::edge* current = begin;

	do
	{
		if (!current->face)
		{
			return current;
		}

		current = current->next->symmetric;
	}
	while (current != begin);

	return nullptr;
}

mesh::edge* mesh::find_free_incident(mesh::edge* start_edge, mesh::edge* end_edge) const
{
	if (start_edge == end_edge)
	{
		return nullptr;
	}

	mesh::edge* current = start_edge;
	do
	{
		if (!current->face)
		{
			return current;
		}

		current = current->next->symmetric;
	}
	while (current != end_edge);

	return nullptr;
}

bool mesh::make_adjacent(mesh::edge* in, mesh::edge* out)
{
	if (in->next == out)
	{
		return true;
	}

	mesh::edge* b = in->next;
	mesh::edge* d = out->previous;
	mesh::edge* g = find_free_incident(out->symmetric, in);
	if (!g)
	{
		return false;
	}

	mesh::edge* h = g->next;

	in->next = out;
	out->previous = in;

	g->next = b;
	b->previous = g;

	d->next = h;
	h->previous = d;

	return true;
}


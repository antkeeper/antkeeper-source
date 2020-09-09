/*
 * Copyright (C) 2020  Christopher J. Howard
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

#ifndef ANTKEEPER_MESH_HPP
#define ANTKEEPER_MESH_HPP

#include <vector>
#include "utility/fundamental-types.hpp"

/**
 * Half-edge mesh.
 *
 * @see http://kaba.hilvi.org/homepage/blog/halfedge/halfedge.htm
 */
class mesh
{
public:
	struct vertex;
	struct edge;
	struct face;
	typedef std::vector<edge*> loop;

	/**
	 * Creates a mesh.
	 */
	mesh() = default;
	
	/**
	 * Destroys a mesh.
	 */
	~mesh();

	/**
	 * Adds a vertex to the mesh. This vertex initially has a null edge.
	 *
	 * @param position Position of the vertex.
	 * @return Pointer to the added vertex.
	 */
	mesh::vertex* add_vertex(const float3& position);

	/**
	 * Adds an edge to the mesh.
	 *
	 * @param a The vertex from which the edge originates.
	 * @param b The vertex at which the edge ends.
	 * @return Pointer to the added edge.
	 */
	mesh::edge* add_edge(mesh::vertex* a, mesh::vertex* b);

	/**
	 * Adds a face to the mesh.
	 *
	 * @param loop List of edges which form the face.
	 * @return Pointer to the added face.
	 */
	mesh::face* add_face(const loop& loop);

	/**
	 * Removes a face from the mesh.
	 *
	 * @param face Face to be removed. This face will be deallocated after removal.
	 */
	void remove_face(mesh::face* face);

	/**
	 * Removes an edge and all dependent faces from the mesh.
	 *
	 * @param edge Edge to be removed. This edge will be deallocated after removal.
	 */
	void remove_edge(mesh::edge* edge);

	/**
	 * Removes a vertex, all dependent edges, and all dependent faces from the mesh.
	 *
	 * @param vertex Vertex to be removed. This vertex will be deallocated after removal.
	 */
	void remove_vertex(mesh::vertex* vertex);
	
	/// Returns the mesh vertices
	const std::vector<mesh::vertex*>& get_vertices() const;
	
	/// Returns the mesh edges
	const std::vector<mesh::edge*>& get_edges() const;
	
	/// Returns the mesh faces
	const std::vector<mesh::face*>& get_faces() const;

	/**
	 * Half-edge vertex which contains a pointer to its parent edge, a position vector, and an index.
	 */
	struct vertex
	{
		/// Pointer to the edge to which this vertex belongs
		mesh::edge* edge;
		
		/// Vertex position
		float3 position;
		
		/// Index of this vertex
		std::size_t index;
	};
	
	/**
	 * Half-edge edge which contains pointers to its starting vertex, parent face, and related edges.
	 */
	struct edge
	{
		/// Pointer to the vertex at which the edge starts
		mesh::vertex* vertex;     
		
		/// Pointer to the face on the left of this edge
		mesh::face* face;
		
		/// Pointer to the previous edge in the parent face
		mesh::edge* previous;
		
		/// Pointer to the next edge in the parent face
		mesh::edge* next;
		
		/// Pointer to the symmetric edge
		mesh::edge* symmetric;
		
		/// Index of this edge
		std::size_t index;
	};
	
	/**
	 * Half-edge face which contains a pointer to its first edge and its normal vector.
	 */
	struct face
	{
		/// Pointer to the first edge in this face
		mesh::edge* edge;
		
		/// Index of this face
		std::size_t index;
	};

private:
	mesh::edge* find_free_incident(mesh::vertex* vertex) const;
	mesh::edge* find_free_incident(mesh::edge* start_edge, mesh::edge* end_edge) const;
	bool make_adjacent(mesh::edge* in_edge, mesh::edge* out_edge);

	std::vector<mesh::vertex*> vertices;
	std::vector<mesh::edge*> edges;
	std::vector<mesh::face*> faces;
};

inline const std::vector<mesh::vertex*>& mesh::get_vertices() const
{
	return vertices;
}

inline const std::vector<mesh::edge*>& mesh::get_edges() const
{
	return edges;
}

inline const std::vector<mesh::face*>& mesh::get_faces() const
{
	return faces;
}

#endif // ANTKEEPER_MESH_HPP


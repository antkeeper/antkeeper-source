/*
 * Copyright (C) 2023  Christopher J. Howard
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

#ifndef ANTKEEPER_GEOM_BREP_MESH_HPP
#define ANTKEEPER_GEOM_BREP_MESH_HPP

#include <engine/geom/brep/brep-vertex.hpp>
#include <engine/geom/brep/brep-edge.hpp>
#include <engine/geom/brep/brep-loop.hpp>
#include <engine/geom/brep/brep-face.hpp>

namespace geom {

/**
 * Boundary representation (B-rep) of a mesh.
 */
class brep_mesh
{
public:
	/**
	 * Constructs an empty mesh.
	 */
	brep_mesh() noexcept;
	
	/**
	 * Constructs a copy of another mesh.
	 *
	 * @param other Mesh to copy.
	 */
	brep_mesh(const brep_mesh& other);
	
	/**
	 * Copies another mesh into this mesh.
	 *
	 * @param other Mesh to copy.
	 *
	 * @return Reference to this mesh.
	 */
	brep_mesh& operator=(const brep_mesh& other);
	
	/// @name Modifiers
	/// @{
	
	/// Erases all vertices, edges, loops, and faces.
	void clear() noexcept;
	
	/// @}
	/// @name Element access
	/// @{
	
	/// Returns the mesh vertices.
	/// @{
	[[nodiscard]] inline const brep_vertex_container& vertices() const noexcept
	{
		return m_vertices;
	}
	[[nodiscard]] inline brep_vertex_container& vertices() noexcept
	{
		return m_vertices;
	}
	///@}
	
	/// Returns the mesh edges.
	/// @{
	[[nodiscard]] inline const brep_edge_container& edges() const noexcept
	{
		return m_edges;
	}
	[[nodiscard]] inline brep_edge_container& edges() noexcept
	{
		return m_edges;
	}
	/// @}
	
	/// Returns the mesh loops.
	/// @{
	[[nodiscard]] inline const brep_loop_container& loops() const noexcept
	{
		return m_loops;
	}
	[[nodiscard]] inline brep_loop_container& loops() noexcept
	{
		return m_loops;
	}
	/// @}
	
	/// Returns the mesh faces.
	/// @{
	[[nodiscard]] inline const brep_face_container& faces() const noexcept
	{
		return m_faces;
	}
	[[nodiscard]] inline brep_face_container& faces() noexcept
	{
		return m_faces;
	}
	/// @}
	
	/// @}
	
private:
	brep_vertex_container m_vertices;
	brep_edge_container m_edges;
	brep_loop_container m_loops;
	brep_face_container m_faces;
};

} // namespace geom

#endif // ANTKEEPER_GEOM_BREP_MESH_HPP

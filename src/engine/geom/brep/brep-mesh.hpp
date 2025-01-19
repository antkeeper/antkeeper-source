// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GEOM_BREP_MESH_HPP
#define ANTKEEPER_GEOM_BREP_MESH_HPP

#include <engine/geom/brep/brep-vertex-container.hpp>
#include <engine/geom/brep/brep-edge-container.hpp>
#include <engine/geom/brep/brep-loop-container.hpp>
#include <engine/geom/brep/brep-face-container.hpp>

namespace geom {

/// Boundary representation (B-rep) of a mesh.
class brep_mesh
{
public:
	/// Constructs an empty mesh.
	brep_mesh() noexcept;
	
	/// Constructs a copy of another mesh.
	/// @param other Mesh to copy.
	brep_mesh(const brep_mesh& other);
	
	/// Copies another mesh into this mesh.
	/// @param other Mesh to copy.
	/// @return Reference to this mesh.
	brep_mesh& operator=(const brep_mesh& other);
	
	/// @name Modifiers
	/// @{
	
	/// Erases all vertices, edges, loops, and faces.
	void clear() noexcept;
	
	/// @}
	/// @name Element access
	/// @{
	
	/// Returns the mesh vertices.
	[[nodiscard]] inline brep_vertex_container& vertices() noexcept
	{
		return m_vertices;
	}

	/// @copydoc vertices()
	[[nodiscard]] inline const brep_vertex_container& vertices() const noexcept
	{
		return m_vertices;
	}

	
	/// Returns the mesh edges.
	[[nodiscard]] inline brep_edge_container& edges() noexcept
	{
		return m_edges;
	}

	/// @copydoc edges()
	[[nodiscard]] inline const brep_edge_container& edges() const noexcept
	{
		return m_edges;
	}

	/// Returns the mesh loops.
	[[nodiscard]] inline brep_loop_container& loops() noexcept
	{
		return m_loops;
	}

	/// @copydoc loops()
	[[nodiscard]] inline const brep_loop_container& loops() const noexcept
	{
		return m_loops;
	}
	
	/// Returns the mesh faces.
	[[nodiscard]] inline brep_face_container& faces() noexcept
	{
		return m_faces;
	}

	/// @copydoc faces()
	[[nodiscard]] inline const brep_face_container& faces() const noexcept
	{
		return m_faces;
	}
	
	/// @}
	
private:
	brep_vertex_container m_vertices;
	brep_edge_container m_edges;
	brep_loop_container m_loops;
	brep_face_container m_faces;
};

} // namespace geom

#endif // ANTKEEPER_GEOM_BREP_MESH_HPP

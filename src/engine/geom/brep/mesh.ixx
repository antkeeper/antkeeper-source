// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.geom.brep.mesh;
export import engine.geom.brep.vertex_container;
export import engine.geom.brep.edge_container;
export import engine.geom.brep.loop_container;
export import engine.geom.brep.face_container;

export namespace engine::geom::brep
{
	/// Boundary representation (B-rep) of a mesh.
	class mesh
	{
	public:
		/// Constructs an empty mesh.
		mesh() noexcept;

		/// Constructs a copy of another mesh.
		/// @param other Mesh to copy.
		mesh(const mesh& other);

		/// Copies another mesh into this mesh.
		/// @param other Mesh to copy.
		/// @return Reference to this mesh.
		mesh& operator=(const mesh& other);

		/// @name Modifiers
		/// @{

		/// Erases all vertices, edges, loops, and faces.
		void clear() noexcept;

		/// @}
		/// @name Element access
		/// @{

		/// Returns the mesh vertices.
		[[nodiscard]] inline vertex_container& vertices() noexcept
		{
			return m_vertices;
		}

		/// @copydoc vertices()
		[[nodiscard]] inline const vertex_container& vertices() const noexcept
		{
			return m_vertices;
		}


		/// Returns the mesh edges.
		[[nodiscard]] inline edge_container& edges() noexcept
		{
			return m_edges;
		}

		/// @copydoc edges()
		[[nodiscard]] inline const edge_container& edges() const noexcept
		{
			return m_edges;
		}

		/// Returns the mesh loops.
		[[nodiscard]] inline loop_container& loops() noexcept
		{
			return m_loops;
		}

		/// @copydoc loops()
		[[nodiscard]] inline const loop_container& loops() const noexcept
		{
			return m_loops;
		}

		/// Returns the mesh faces.
		[[nodiscard]] inline face_container& faces() noexcept
		{
			return m_faces;
		}

		/// @copydoc faces()
		[[nodiscard]] inline const face_container& faces() const noexcept
		{
			return m_faces;
		}

		/// @}

	private:
		vertex_container m_vertices;
		edge_container m_edges;
		loop_container m_loops;
		face_container m_faces;
	};
}

// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.geom.brep.vertex;
export import engine.geom.brep.vertex_edge_list;
import engine.utility.sized_types;

export namespace engine::geom::brep
{
	class mesh;
	template <class T>
	class element_container;

	/// A point in space.
	class vertex
	{
	public:
		/// Returns the index of this vertex in the mesh vertex array.
		/// @warning This index may change if any vertices are removed from the mesh.
		[[nodiscard]] inline constexpr usize index() const noexcept
		{
			return m_index;
		}

		/// Returns the list of edges bounded by this vertex.
		[[nodiscard]] inline constexpr const vertex_edge_list& edges() const noexcept
		{
			return m_edges;
		}

	private:
		friend class mesh;
		friend class element_container<vertex>;
		friend class vertex_container;
		friend class edge_container;

		usize m_index{};
		vertex_edge_list m_edges;
	};
}

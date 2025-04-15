// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.geom.brep.edge_container;
export import engine.geom.brep.element_container;
export import engine.geom.brep.edge;

export namespace engine::geom::brep
{
	class vertex;

	/// B-rep edge container.
	class edge_container: public element_container<edge>
	{
	public:
		/// Destructs a B-rep edge container.
		~edge_container() override = default;

		/// Appends a new edge to the end of the container.
		/// @param a First bounding vertex of the edge.
		/// @param b Second bounding vertex of the edge.
		/// @return Pointer to the new edge.
		edge* emplace_back(vertex* a, vertex* b);

		/// Erases an edge and all dependent loops and faces.
		/// @param edge Pointer to the edge to erase.
		/// @warning Invalidates iterators and indices of edges, loops, and faces.
		void erase(brep::edge* edge) override;

		/// Erases all edges and their dependent loops and faces.
		void clear() noexcept;

		/// Finds an edge bounded by vertices @p a and @p b (in any order).
		/// @param a First (or second) bounding vertex of the edge.
		/// @param b Second (or first) bounding vertex of the edge.
		/// @return Pointer to an edge bounded by vertices @p a and @p b, or `nullptr` if no such edge was found.
		[[nodiscard]] edge* find(vertex* a, vertex* b) const;

	private:
		friend class mesh;

		// Suppress -Werror=overloaded-virtual
		using element_container<edge>::emplace_back;
	};
}

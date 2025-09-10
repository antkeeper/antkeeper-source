// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <engine/geom/brep/element-container.hpp>
#include <engine/geom/brep/vertex.hpp>

namespace engine::geom::brep
{
	/// B-rep vertex container.
	class vertex_container: public element_container<vertex>
	{
	public:
		/// Destructs a B-rep vertex container.
		~vertex_container() override = default;

		/// Appends a new vertex to the end of the container.
		/// @return Pointer to the new vertex.
		vertex* emplace_back() override;

		/// Erases a vertex and all dependent edges, loops, and faces.
		/// @param vertex Pointer to the vertex to erase.
		/// @warning Invalidates iterators and indices of vertices, edges, loops, and faces.
		void erase(brep::vertex* vertex) override;

		/// Erases all vertices and their dependent edges, loops, and faces.
		void clear() noexcept;

	private:
		friend class mesh;
	};
}

// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <engine/geom/brep/edge-loop-list.hpp>
#include <engine/utility/sized-types.hpp>
#include <array>

namespace engine::geom::brep
{
	class vertex;
	template <class T>
	class element_container;

	/// Curve segment bounded by two vertices.
	class edge
	{
	public:
		friend class mesh;
		friend class vertex_edge_list;
		friend class element_container<edge>;
		friend class edge_container;
		friend class face_container;

		/// Returns the index of this edge in the mesh edge array.
		/// @warning This index may change if any edges are removed from the mesh.
		[[nodiscard]] inline constexpr usize index() const noexcept
		{
			return m_index;
		}

		/// Returns the pair of vertices that bound this edge.
		[[nodiscard]] inline constexpr const std::array<vertex*, 2>& vertices() const noexcept
		{
			return m_vertices;
		}

		/// Returns the list of loops that share this edge.
		[[nodiscard]] inline constexpr const edge_loop_list& loops() const noexcept
		{
			return m_loops;
		}

	private:
		usize m_index{};
		std::array<vertex*, 2> m_vertices{};
		std::array<edge*, 2> m_vertex_next{};
		std::array<edge*, 2> m_vertex_previous{};
		edge_loop_list m_loops;
	};
}

// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <engine/utility/sized-types.hpp>

namespace engine::geom::brep
{
	class vertex;
	class edge;
	class face;
	template <class T>
	class element_container;

	/// Connected boundary of a single face.
	class loop
	{
	public:
		friend class mesh;
		friend class edge_loop_list;
		friend class face_loop_list;
		friend class element_container<loop>;
		friend class loop_container;
		friend class face_container;

		/// Returns the index of this loop in the mesh loop array.
		/// @warning This index may change if any loops are removed from the mesh.
		[[nodiscard]] inline constexpr auto index() const noexcept
		{
			return m_index;
		}

		/// Returns a pointer to the loop vertex.
		[[nodiscard]] inline constexpr auto vertex() const noexcept
		{
			return m_vertex;
		}

		/// Returns a pointer to the loop edge.
		[[nodiscard]] inline constexpr auto edge() const noexcept
		{
			return m_edge;
		}

		/// Returns a pointer to the loop face.
		[[nodiscard]] inline constexpr auto face() const noexcept
		{
			return m_face;
		}

		/// Returns a pointer to the next loop.
		[[nodiscard]] inline constexpr auto next() const noexcept
		{
			return m_face_next;
		}

		/// Returns a pointer to the previous loop.
		[[nodiscard]] inline constexpr auto previous() const noexcept
		{
			return m_face_previous;
		}

	private:
		usize m_index{};
		brep::vertex* m_vertex{};
		brep::edge* m_edge{};
		brep::face* m_face{};
		brep::loop* m_edge_next{};
		brep::loop* m_edge_previous{};
		brep::loop* m_face_next{};
		brep::loop* m_face_previous{};
	};
}

// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <engine/geom/brep/face-loop-list.hpp>
#include <engine/utility/sized-types.hpp>

namespace engine::geom::brep
{
	template <class T>
	class element_container;

	/// Portion of a shell bounded by loops.
	class face
	{
	public:
		friend class mesh;
		friend class element_container<face>;
		friend class face_container;

		/// Returns the index of this face in the mesh face array.
		/// @warning This index may change if any faces are removed from the mesh.
		[[nodiscard]] inline constexpr usize index() const noexcept
		{
			return m_index;
		}

		/// Returns the list of loops associated with this face.
		[[nodiscard]] inline constexpr const face_loop_list& loops() const noexcept
		{
			return m_loops;
		}

	private:
		usize m_index{};
		face_loop_list m_loops;
	};
}

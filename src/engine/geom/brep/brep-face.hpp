// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GEOM_BREP_FACE_HPP
#define ANTKEEPER_GEOM_BREP_FACE_HPP

#include <engine/geom/brep/brep-face-loop-list.hpp>
#include <cstddef>

namespace geom {

template <class T>
class brep_element_container;

/// Portion of a shell bounded by loops.
class brep_face
{
public:
	friend class brep_mesh;
	friend class brep_element_container<brep_face>;
	friend class brep_face_container;
	
	/// Returns the index of this face in the mesh face array.
	/// @warning This index may change if any faces are removed from the mesh.
	[[nodiscard]] inline constexpr std::size_t index() const noexcept
	{
		return m_index;
	}
	
	/// Returns the list of loops associated with this face.
	[[nodiscard]] inline constexpr const brep_face_loop_list& loops() const noexcept
	{
		return m_loops;
	}
	
private:
	std::size_t m_index{};
	brep_face_loop_list m_loops;
};

} // namespace geom

#endif // ANTKEEPER_GEOM_BREP_FACE_HPP

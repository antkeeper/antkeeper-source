// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GEOM_BREP_LOOP_HPP
#define ANTKEEPER_GEOM_BREP_LOOP_HPP

#include <cstddef>

namespace geom {

class brep_vertex;
class brep_edge;
class brep_face;
template <class T>
class brep_element_container;

/// Connected boundary of a single face.
class brep_loop
{
public:
	friend class brep_mesh;
	friend class brep_edge_loop_list;
	friend class brep_face_loop_list;
	friend class brep_element_container<brep_loop>;
	friend class brep_loop_container;
	friend class brep_face_container;
	
	/// Returns the index of this loop in the mesh loop array.
	/// @warning This index may change if any loops are removed from the mesh.
	[[nodiscard]] inline constexpr std::size_t index() const noexcept
	{
		return m_index;
	}
	
	/// Returns a pointer to the loop vertex.
	[[nodiscard]] inline constexpr brep_vertex* vertex() const noexcept
	{
		return m_vertex;
	}
	
	/// Returns a pointer to the loop edge.
	[[nodiscard]] inline constexpr brep_edge* edge() const noexcept
	{
		return m_edge;
	}
	
	/// Returns a pointer to the loop face.
	[[nodiscard]] inline constexpr brep_face* face() const noexcept
	{
		return m_face;
	}
	
	/// Returns a pointer to the next loop.
	[[nodiscard]] inline constexpr brep_loop* next() const noexcept
	{
		return m_face_next;
	}
	
	/// Returns a pointer to the previous loop.
	[[nodiscard]] inline constexpr brep_loop* previous() const noexcept
	{
		return m_face_previous;
	}
	
private:
	std::size_t m_index{};
	brep_vertex* m_vertex{};
	brep_edge* m_edge{};
	brep_face* m_face{};
	brep_loop* m_edge_next{};
	brep_loop* m_edge_previous{};
	brep_loop* m_face_next{};
	brep_loop* m_face_previous{};
};

} // namespace geom

#endif // ANTKEEPER_GEOM_BREP_LOOP_HPP

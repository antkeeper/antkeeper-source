// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GEOM_BREP_EDGE_HPP
#define ANTKEEPER_GEOM_BREP_EDGE_HPP

#include <engine/geom/brep/brep-edge-loop-list.hpp>
#include <array>
#include <cstddef>

namespace geom {

class brep_vertex;
template <class T>
class brep_element_container;

/// Curve segment bounded by two vertices.
class brep_edge
{
public:
	friend class brep_mesh;
	friend class brep_vertex_edge_list;
	friend class brep_element_container<brep_edge>;
	friend class brep_edge_container;
	friend class brep_face_container;
	
	/// Returns the index of this edge in the mesh edge array.
	/// @warning This index may change if any edges are removed from the mesh.
	[[nodiscard]] inline constexpr std::size_t index() const noexcept
	{
		return m_index;
	}
	
	/// Returns the pair of vertices that bound this edge.
	[[nodiscard]] inline constexpr const std::array<brep_vertex*, 2>& vertices() const noexcept
	{
		return m_vertices;
	}
	
	/// Returns the list of loops that share this edge.
	[[nodiscard]] inline constexpr const brep_edge_loop_list& loops() const noexcept
	{
		return m_loops;
	}
	
private:
	std::size_t m_index{};
	std::array<brep_vertex*, 2> m_vertices{};
	std::array<brep_edge*, 2> m_vertex_next{};
	std::array<brep_edge*, 2> m_vertex_previous{};
	brep_edge_loop_list m_loops;
};

} // namespace geom

#endif // ANTKEEPER_GEOM_BREP_EDGE_HPP

// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GEOM_BREP_VERTEX_HPP
#define ANTKEEPER_GEOM_BREP_VERTEX_HPP

#include <engine/geom/brep/brep-vertex-edge-list.hpp>
#include <cstddef>

namespace geom {

class brep_mesh;
template <class T>
class brep_element_container;

/// A point in space.
class brep_vertex
{
public:
	/// Returns the index of this vertex in the mesh vertex array.
	/// @warning This index may change if any vertices are removed from the mesh.
	[[nodiscard]] inline constexpr std::size_t index() const noexcept
	{
		return m_index;
	}
	
	/// Returns the list of edges bounded by this vertex.
	[[nodiscard]] inline constexpr const brep_vertex_edge_list& edges() const noexcept
	{
		return m_edges;
	}
	
private:
	friend class brep_mesh;
	friend class brep_element_container<brep_vertex>;
	friend class brep_vertex_container;
	friend class brep_edge_container;

	std::size_t m_index{};
	brep_vertex_edge_list m_edges;
};

} // namespace geom

#endif // ANTKEEPER_GEOM_BREP_VERTEX_HPP

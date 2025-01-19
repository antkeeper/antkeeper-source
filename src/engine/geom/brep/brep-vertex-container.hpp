// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GEOM_BREP_VERTEX_CONTAINER_HPP
#define ANTKEEPER_GEOM_BREP_VERTEX_CONTAINER_HPP

#include <engine/geom/brep/brep-element-container.hpp>
#include <engine/geom/brep/brep-vertex.hpp>

namespace geom {

/// B-rep vertex container.
class brep_vertex_container: public brep_element_container<brep_vertex>
{
public:
	/// Destructs a B-rep vertex container.
	~brep_vertex_container() override = default;
	
	/// Appends a new vertex to the end of the container.
	/// @return Pointer to the new vertex.
	brep_vertex* emplace_back() override;
	
	/// Erases a vertex and all dependent edges, loops, and faces.
	/// @param vertex Pointer to the vertex to erase.
	/// @warning Invalidates iterators and indices of vertices, edges, loops, and faces.
	void erase(brep_vertex* vertex) override;
	
	/// Erases all vertices and their dependent edges, loops, and faces.
	void clear() noexcept;
	
private:
	friend class brep_mesh;
};

} // namespace geom

#endif // ANTKEEPER_GEOM_BREP_VERTEX_CONTAINER_HPP

// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GEOM_BREP_EDGE_CONTAINER_HPP
#define ANTKEEPER_GEOM_BREP_EDGE_CONTAINER_HPP

#include <engine/geom/brep/brep-element-container.hpp>
#include <engine/geom/brep/brep-edge.hpp>

namespace geom {

class brep_vertex;

/**
 * B-rep edge container.
 */
class brep_edge_container: public brep_element_container<brep_edge>
{
public:
	/** Destructs a B-rep edge container. */
	~brep_edge_container() override = default;
	
	/**
	 * Appends a new edge to the end of the container.
	 *
	 * @param a First bounding vertex of the edge.
	 * @param b Second bounding vertex of the edge.
	 *
	 * @return Pointer to the new edge.
	 */
	brep_edge* emplace_back(brep_vertex* a, brep_vertex* b);
	
	/**
	 * Erases an edge and all dependent loops and faces.
	 *
	 * @param edge Pointer to the edge to erase.
	 *
	 * @warning Invalidates iterators and indices of edges, loops, and faces.
	 */
	void erase(brep_edge* edge) override;
	
	/**
	 * Erases all edges and their dependent loops and faces.
	 */
	void clear() noexcept;
	
	/**
	 * Finds an edge bounded by vertices @p a and @p b (in any order).
	 *
	 * @param a First (or second) bounding vertex of the edge.
	 * @param b Second (or first) bounding vertex of the edge.
	 *
	 * @return Pointer to an edge bounded by vertices @p a and @p b, or `nullptr` if no such edge was found.
	 */
	[[nodiscard]] brep_edge* find(brep_vertex* a, brep_vertex* b) const;
	
private:
	friend class brep_mesh;
};

} // namespace geom

#endif // ANTKEEPER_GEOM_BREP_EDGE_CONTAINER_HPP

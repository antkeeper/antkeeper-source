/*
 * Copyright (C) 2023  Christopher J. Howard
 *
 * This file is part of Antkeeper source code.
 *
 * Antkeeper source code is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Antkeeper source code is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Antkeeper source code.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef ANTKEEPER_GEOM_BREP_LOOP_HPP
#define ANTKEEPER_GEOM_BREP_LOOP_HPP

#include <engine/geom/brep/brep-element-container.hpp>
#include <cstddef>

namespace geom {

class brep_vertex;
class brep_edge;
class brep_face;
template <class T>
class brep_element_container;

/**
 * Connected boundary of a single face.
 */
class brep_loop
{
public:
	friend class brep_mesh;
	friend class brep_edge_loop_list;
	friend class brep_face_loop_list;
	friend class brep_element_container<brep_loop>;
	friend class brep_loop_container;
	friend class brep_face_container;
	
	/**
	 * Returns the index of this loop in the mesh loop array.
	 *
	 * @warning This index may change if any loops are removed from the mesh.
	 */
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
	
private:
	std::size_t m_index;
	brep_vertex* m_vertex;
	brep_edge* m_edge;
	brep_face* m_face;
	brep_loop* m_edge_next;
	brep_loop* m_edge_previous;
	brep_loop* m_face_next;
	brep_loop* m_face_previous;
};

/**
 * B-rep loop container.
 */
class brep_loop_container: public brep_element_container<brep_loop>
{
private:
	friend class brep_mesh;
	friend class brep_face_container;
	
	/**
	 * Constructs a B-rep face container.
	 *
	 * @param mesh Pointer to the parent mesh.
	 */
	inline explicit brep_loop_container(brep_mesh* mesh) noexcept:
		brep_element_container<brep_loop>(mesh)
	{}
	
	/**
	 * Appends a new loop to the end of the container.
	 *
	 * @return Pointer to the new loop.
	 */
	brep_loop* emplace_back() override;
};

} // namespace geom

#endif // ANTKEEPER_GEOM_BREP_LOOP_HPP

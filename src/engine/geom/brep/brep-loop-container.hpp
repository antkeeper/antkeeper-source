// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GEOM_BREP_LOOP_CONTAINER_HPP
#define ANTKEEPER_GEOM_BREP_LOOP_CONTAINER_HPP

#include <engine/geom/brep/brep-element-container.hpp>
#include <engine/geom/brep/brep-loop.hpp>

namespace geom {

/// B-rep loop container.
class brep_loop_container: public brep_element_container<brep_loop>
{
private:
	friend class brep_mesh;
	friend class brep_face_container;
	
	/// Destructs a B-rep loop container.
	~brep_loop_container() override = default;
	
	/// Appends a new loop to the end of the container.
	/// @return Pointer to the new loop.
	brep_loop* emplace_back() override;
};

} // namespace geom

#endif // ANTKEEPER_GEOM_BREP_LOOP_CONTAINER_HPP

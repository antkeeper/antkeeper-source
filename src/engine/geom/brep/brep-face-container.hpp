// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GEOM_BREP_FACE_CONTAINER_HPP
#define ANTKEEPER_GEOM_BREP_FACE_CONTAINER_HPP

#include <engine/geom/brep/brep-element-container.hpp>
#include <engine/geom/brep/brep-face.hpp>
#include <cstddef>
#include <span>

namespace geom {

/// B-rep face container.
class brep_face_container: public brep_element_container<brep_face>
{
public:
	/// Destructs a B-rep face container.
	~brep_face_container() override = default;
	
	/// Appends a new face to the end of the container.
	/// @param vertices Ordered vertices of the face.
	/// @return Pointer to the new face.
	brep_face* emplace_back(const std::span<brep_vertex*> vertices);
	
	/// Erases a face and all of its loops.
	/// @param face Pointer to the face to erase.
	/// @warning Invalidates iterators and indices of loops and faces.
	void erase(brep_face* face) override;
	
	/// Erases all faces and their loops.
	void clear() noexcept;
	
	/// Reverses the direction of a face's bounding loops.
	/// @param face Face bounded by the loops to reverse.
	void reverse(brep_face* face);
	
private:
	friend class brep_mesh;
};

} // namespace geom

#endif // ANTKEEPER_GEOM_BREP_FACE_CONTAINER_HPP

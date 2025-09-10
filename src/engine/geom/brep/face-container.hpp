// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <engine/geom/brep/element-container.hpp>
#include <engine/geom/brep/face.hpp>
#include <span>

namespace engine::geom::brep
{
	/// B-rep face container.
	class face_container: public element_container<face>
	{
	public:
		/// Destructs a B-rep face container.
		~face_container() override = default;

		/// Appends a new face to the end of the container.
		/// @param vertices Ordered vertices of the face.
		/// @return Pointer to the new face.
		face* emplace_back(const std::span<vertex*> vertices);

		/// Erases a face and all of its loops.
		/// @param face Pointer to the face to erase.
		/// @warning Invalidates iterators and indices of loops and faces.
		void erase(brep::face* face) override;

		/// Erases all faces and their loops.
		void clear() noexcept;

		/// Reverses the direction of a face's bounding loops.
		/// @param face Face bounded by the loops to reverse.
		void reverse(brep::face* face);

	private:
		friend class mesh;

		// Suppress -Werror=overloaded-virtual
		using element_container<face>::emplace_back;
	};
}

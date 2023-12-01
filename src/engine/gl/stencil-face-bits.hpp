// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GL_STENCIL_FACE_BITS_HPP
#define ANTKEEPER_GL_STENCIL_FACE_BITS_HPP

#include <cstdint>

namespace gl {

/// Stencil face mask bits specifying which stencil state(s) to update.
enum: std::uint8_t
{
	/// Only the front set of stencil state is updated.
	stencil_face_front_bit = 0b01,
	
	/// Only the back set of stencil state is updated.
	stencil_face_back_bit = 0b10,
	
	/// Both sets of stencil state are updated.
	stencil_face_front_and_back = 0b11,
};

} // namespace gl

#endif // ANTKEEPER_GL_STENCIL_FACE_BITS_HPP

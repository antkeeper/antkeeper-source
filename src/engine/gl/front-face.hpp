// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GL_FRONT_FACE_HPP
#define ANTKEEPER_GL_FRONT_FACE_HPP

#include <cstdint>

namespace gl {

/// Polygon front-facing orientation.
enum class front_face: std::uint8_t
{
	/// Triangle with positive area is considered front-facing.
	counter_clockwise,
	
	/// Triangle with negative area is considered front-facing.
	clockwise
};

} // namespace gl

#endif // ANTKEEPER_GL_FRONT_FACE_HPP

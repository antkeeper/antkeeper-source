// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GL_CULL_MODE_HPP
#define ANTKEEPER_GL_CULL_MODE_HPP

#include <cstdint>

namespace gl {

/// Triangle culling mode.
enum class cull_mode: std::uint8_t
{
	/// No triangles are discarded.
	none,
	
	/// Front-facing triangles are discarded.
	front,
	
	/// Back-facing triangles are discarded.
	back,
	
	/// All triangles are discarded.
	front_and_back
};

} // namespace gl

#endif // ANTKEEPER_GL_CULL_MODE_HPP

// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GL_VERTEX_INPUT_RATE_HPP
#define ANTKEEPER_GL_VERTEX_INPUT_RATE_HPP

#include <cstdint>

namespace gl {

/// Rate at which vertex attributes are pulled from buffers.
enum class vertex_input_rate: std::uint8_t
{
	/// Vertex attribute addressing is a function of the vertex index.
	vertex,
	
	/// Vertex attribute addressing is a function of the instance index.
	instance
};

} // namespace gl

#endif // ANTKEEPER_GL_VERTEX_INPUT_RATE_HPP

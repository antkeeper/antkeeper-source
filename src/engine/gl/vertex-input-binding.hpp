// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GL_VERTEX_INPUT_BINDING_HPP
#define ANTKEEPER_GL_VERTEX_INPUT_BINDING_HPP

#include <engine/gl/vertex-input-rate.hpp>
#include <cstdint>

namespace gl {

/// Vertex input binding.
struct vertex_input_binding
{
	/// Binding number that this structure describes.
	std::uint32_t binding{0};
	
	/// Byte stride between consecutive elements within the buffer.
	std::uint32_t stride{0};
	
	/// Specifies whether vertex attribute addressing is a function of the vertex index or of the instance index.
	vertex_input_rate input_rate{vertex_input_rate::vertex};
};

} // namespace gl

#endif // ANTKEEPER_GL_VERTEX_INPUT_BINDING_HPP

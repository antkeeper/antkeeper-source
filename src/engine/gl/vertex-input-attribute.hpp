// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GL_VERTEX_INPUT_ATTRIBUTE_HPP
#define ANTKEEPER_GL_VERTEX_INPUT_ATTRIBUTE_HPP

#include <engine/gl/format.hpp>
#include <cstdint>

namespace gl {

/// Vertex input attribute.
struct vertex_input_attribute
{
	/// Shader input location number for this attribute.
	std::uint32_t location{0};
	
	/// Binding number which this attribute takes its data from.
	std::uint32_t binding{0};
	
	/// Size and type of the vertex attribute data.
	gl::format format{gl::format::undefined};
	
	/// Byte offset of this attribute relative to the start of an element in the vertex input binding.
	std::uint32_t offset{0};
};

} // namespace gl

#endif // ANTKEEPER_GL_VERTEX_INPUT_ATTRIBUTE_HPP

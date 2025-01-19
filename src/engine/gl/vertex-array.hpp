// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GL_VERTEX_ARRAY_HPP
#define ANTKEEPER_GL_VERTEX_ARRAY_HPP

#include <engine/gl/vertex-input-attribute.hpp>
#include <span>
#include <vector>

namespace gl {

/// Vertex arrays describes how vertex input attributes are stored in vertex buffers.
class vertex_array
{
public:
	/// Constructs a vertex array.
	/// @param attributes Vertex input attributes.
	/// @exception std::invalid_argument Vertex input attribute has unsupported format.
	explicit vertex_array(std::span<const vertex_input_attribute> attributes);

	/// Constructs an empty vertex array.
	vertex_array();
	
	/// Destructs a vertex array.
	~vertex_array();
	
	/// Returns the vertex array's vertex input attributes.
	[[nodiscard]] inline constexpr const std::vector<vertex_input_attribute>& attributes() const noexcept
	{
		return m_attributes;
	}
	
	vertex_array(const vertex_array&) = delete;
	vertex_array(vertex_array&&) = delete;
	vertex_array& operator=(const vertex_array&) = delete;
	vertex_array& operator=(vertex_array&&) = delete;
	
private:
	friend class pipeline;
	
	std::vector<vertex_input_attribute> m_attributes;
	unsigned int m_gl_named_array{0};
};

} // namespace gl

#endif // ANTKEEPER_GL_VERTEX_ARRAY_HPP

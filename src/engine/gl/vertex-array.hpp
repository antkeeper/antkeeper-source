/*
 * Copyright (C) 2023  Christopher J. Howard
 *
 * This file is part of Antkeeper source code.
 *
 * Antkeeper source code is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Antkeeper source code is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Antkeeper source code.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef ANTKEEPER_GL_VERTEX_ARRAY_HPP
#define ANTKEEPER_GL_VERTEX_ARRAY_HPP

#include <engine/gl/vertex-input-attribute.hpp>
#include <span>
#include <vector>

namespace gl {

/**
 * Vertex arrays describes how vertex input attributes are stored in vertex buffers.
 */
class vertex_array
{
public:
	/**
	 * Constructs a vertex array.
	 *
	 * @param attributes Vertex input attributes.
	 *
	 * @except std::invalid_argument Vertex input attribute has unsupported format.
	 */
	/// @{
	explicit vertex_array(std::span<const vertex_input_attribute> attributes);
	vertex_array();
	/// @}
	
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

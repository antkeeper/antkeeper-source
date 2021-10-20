/*
 * Copyright (C) 2021  Christopher J. Howard
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

#include <cstdint>
#include <unordered_map>
#include "gl/vertex-attribute.hpp"

namespace gl {

class rasterizer;
class vertex_buffer;

/**
 * Vertex array object (VAO), which describes how vertex attributes are stored in vertex buffer objects (VBOs).
 *
 * @see gl::vertex_attribute
 * @see gl::vertex_buffer
 */
class vertex_array
{
public:
	/// Vertex attribute binding location type.
	typedef std::uint_fast32_t attribute_location_type;
	
	/// Maps vertex attribute to binding locations.
	typedef std::unordered_map<attribute_location_type, vertex_attribute> attribute_map_type;
	
	/// Constructs a vertex array.
	vertex_array();
	
	/// Destructs a vertex array.
	~vertex_array();

	vertex_array(const vertex_array&) = delete;
	vertex_array& operator=(const vertex_array&) = delete;
	
	/**
	 * Binds a vertex attribute to the vertex array.
	 *
	 * @param location Location to which the vertex attribute should be bound.
	 * @param attribute Vertex attribute to bind.
	 *
	 * @except std::invalid_argument Cannot bind vertex attribute that has a null vertex buffer.
	 * @except std::invalid_argument Cannot bind vertex attribute that has an unsupported number of components.
	 */
	void bind(attribute_location_type location, const vertex_attribute& attribute);
	
	/**
	 * Unbinds a vertex attribute from the vertex array.
	 *
	 * @param location Location of the vertex attribute to unbind.
	 *
	 * @except std::invalid_argument Non-existent vertex attribute cannot be unbound.
	 */
	void unbind(attribute_location_type location);
	
	/// Returns a const reference to the map of vertex attributes bound to this vertex array.
	const attribute_map_type& get_attributes() const;
	
private:
	friend class rasterizer;
	
	attribute_map_type attributes;
	std::uint_fast32_t gl_array_id;
};

} // namespace gl

#endif // ANTKEEPER_GL_VERTEX_ARRAY_HPP

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

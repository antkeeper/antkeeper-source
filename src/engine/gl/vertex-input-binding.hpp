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

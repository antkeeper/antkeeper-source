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

#ifndef ANTKEEPER_GL_PRIMITIVE_TOPOLOGY_HPP
#define ANTKEEPER_GL_PRIMITIVE_TOPOLOGY_HPP

#include <cstdint>

namespace gl {

/// Primitive topologies.
enum class primitive_topology: std::uint8_t
{
	/// Separate point primitives.
	point_list,
	
	/// Separate line primitives.
	line_list,
	
	/// Connected line primitives with consecutive lines sharing a vertex.
	line_strip,
	
	/// Separate triangle primitives.
	triangle_list,
	
	/// Connected triangle primitives with consecutive triangles sharing an edge.
	triangle_strip,
	
	/// Connected triangle primitives with all triangles sharing a common vertex.
	triangle_fan,
	
	/// Separate line primitives with adjacency.
	line_list_with_adjacency,
	
	/// Connected line primitives with adjacency, with consecutive primitives sharing three vertices.
	line_strip_with_adjacency,
	
	/// Separate triangle primitives with adjacency.
	triangle_list_with_adjacency,
	
	/// Connected triangle primitives with adjacency, with consecutive triangles sharing an edge.
	triangle_strip_with_adjacency,
	
	/// Separate patch primitives.
	patch_list
};

} // namespace gl

#endif // ANTKEEPER_GL_PRIMITIVE_TOPOLOGY_HPP

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

#ifndef ANTKEEPER_RENDER_VERTEX_ATTRIBUTE_HPP
#define ANTKEEPER_RENDER_VERTEX_ATTRIBUTE_HPP

namespace render {

/**
 * Contains enumerated vertex attribute locations.
 */
namespace vertex_attribute
{
	enum
	{
		/// Vertex position (vec3)
		position,
		
		/// Vertex UV texture coordinates (vec2)
		uv,
		
		/// Vertex normal (vec3)
		normal,
		
		/// Vertex tangent (vec4)
		tangent,
		
		/// Vertex color (vec4)
		color,
		
		/// Vertex bone indices (vec4)
		bone_index,
		
		/// Vertex bone weights (vec4)
		bone_weight,
		
		/// Vertex barycentric coordinates (vec3)
		barycentric,
		
		/// Vertex morph target (vec3)
		target,
		
		/// General purpose index (uint)
		index
	};
}

} // namespace render

#endif // ANTKEEPER_RENDER_VERTEX_ATTRIBUTE_HPP

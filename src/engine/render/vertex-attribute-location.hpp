// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_RENDER_VERTEX_ATTRIBUTE_LOCATION_HPP
#define ANTKEEPER_RENDER_VERTEX_ATTRIBUTE_LOCATION_HPP

#include <cstdint>

namespace render {

/// Vertex attribute locations.
namespace vertex_attribute_location
{
	enum: std::uint8_t
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
		
		/// Vertex bone indices (uvec4)
		bone_index,
		
		/// Vertex bone weights (vec4)
		bone_weight,
		
		/// Vertex barycentric coordinates (vec3)
		barycentric,
		
		/// Vertex morph target (vec3)
		target
	};
}

} // namespace render

#endif // ANTKEEPER_RENDER_VERTEX_ATTRIBUTE_LOCATION_HPP

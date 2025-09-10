// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once


namespace engine::render
{
	/// Vertex attribute locations.
	namespace vertex_attribute_location
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
}

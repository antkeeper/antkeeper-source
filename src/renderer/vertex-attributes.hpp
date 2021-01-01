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

#ifndef ANTKEEPER_VERTEX_ATTRIBUTES_HPP
#define ANTKEEPER_VERTEX_ATTRIBUTES_HPP

/// Vertex position (vec3)
#define VERTEX_POSITION_LOCATION 0

/// Vertex texture coordinates (vec2)
#define VERTEX_TEXCOORD_LOCATION 1

/// Vertex normal (vec3)
#define VERTEX_NORMAL_LOCATION 2

/// Vertex tangent (vec4)
#define VERTEX_TANGENT_LOCATION 3

/// Vertex color (vec4)
#define VERTEX_COLOR_LOCATION 4

/// Vertex bone indices (vec4)
#define VERTEX_BONE_INDEX_LOCATION 5

/// Vertex bone weights (vec4)
#define VERTEX_BONE_WEIGHT_LOCATION 6

/// Vertex barycentric coordinates (vec3)
#define VERTEX_BARYCENTRIC_LOCATION 7

#endif // ANTKEEPER_VERTEX_ATTRIBUTES_HPP


/*
 * Copyright (C) 2020  Christopher J. Howard
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

#ifndef ANTKEEPER_MESH_FUNCTIONS_HPP
#define ANTKEEPER_MESH_FUNCTIONS_HPP

#include "mesh.hpp"
#include <vmq/vmq.hpp>
#include <array>
#include <vector>
#include "geometry/aabb.hpp"

using namespace vmq::types;

/**
 * Creates a triangle mesh from a list of vertices and indices.
 *
 * @param[out] mesh Mesh to which vertices, edges, and faces will be added.
 * @param vertices Vertex positions
 * @param triangles Triangle indices
 * @return Newly created triangle mesh.
 */
void create_triangle_mesh(mesh& mesh, const std::vector<float3>& vertices, const std::vector<std::array<std::uint_fast32_t, 3>>& triangles);

/**
 * Calculates normals for each face.
 *
 * @param[out] Array in which normals will be stored. Must be able to hold `3 * face count` floats.
 * @param layer Face layer in which the normals will be stored. Must have three components.
 */
void calculate_face_normals(float* normals, const mesh& mesh);

void calculate_vertex_normals(float* normals, const mesh& mesh);

/**
 * Calculates the AABB bounds of a mesh.
 */
aabb<float> calculate_bounds(const mesh& mesh);


#endif // ANTKEEPER_MESH_FUNCTIONS_HPP


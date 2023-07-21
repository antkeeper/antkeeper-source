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

#ifndef ANTKEEPER_GEOM_BREP_OPERATIONS_HPP
#define ANTKEEPER_GEOM_BREP_OPERATIONS_HPP

#include <engine/geom/brep/brep-mesh.hpp>

namespace geom {

/**
 * Generates the math::fvec3 face attribute "normal" for a B-rep mesh.
 *
 * @param mesh Mesh for which to generate normals.
 *
 * @warning Requires the math::fvec3 vertex attribute "position".
 */
void generate_face_normals(brep_mesh& mesh);

/**
 * Generates the math::fvec3 vertex attribute "normal" for a B-rep mesh.
 *
 * @param mesh Mesh for which to generate normals.
 *
 * @warning Requires the math::fvec3 vertex attribute "position".
 */
void generate_vertex_normals(brep_mesh& mesh);

/**
 * Generates the math::fvec3 loop attribute "barycentric" for a B-rep mesh.
 *
 * @param mesh Mesh for which to generate barycentric coordinates.
 */
void generate_loop_barycentric(brep_mesh& mesh);

} // namespace geom

#endif // ANTKEEPER_GEOM_BREP_OPERATIONS_HPP

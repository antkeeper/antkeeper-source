// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

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
 * @note The math::fvec3 face attribute "normal" will also be generated if not found.
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

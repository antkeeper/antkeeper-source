// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <engine/geom/brep/mesh.hpp>

namespace engine::geom::brep
{
	/// Generates the math::fvec3 face attribute "normal" for a B-rep mesh.
	/// @param mesh Mesh for which to generate normals.
	/// @warning Requires the math::fvec3 vertex attribute "position".
	void generate_face_normals(mesh& mesh);

	/// Generates the math::fvec3 vertex attribute "normal" for a B-rep mesh.
	/// @param mesh Mesh for which to generate normals.
	/// @note The math::fvec3 face attribute "normal" will also be generated if not found.
	/// @warning Requires the math::fvec3 vertex attribute "position".
	void generate_vertex_normals(mesh& mesh);

	/// Generates the math::fvec3 loop attribute "barycentric" for a B-rep mesh.
	/// @param mesh Mesh for which to generate barycentric coordinates.
	void generate_loop_barycentric(mesh& mesh);
}

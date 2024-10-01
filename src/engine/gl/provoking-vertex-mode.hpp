// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GL_PROVOKING_VERTEX_MODE_HPP
#define ANTKEEPER_GL_PROVOKING_VERTEX_MODE_HPP

#include <cstdint>

namespace gl {

/// Vertex to be used as the source of data for flat-shaded varyings.
enum class provoking_vertex_mode: std::uint8_t
{
	/// Provoking vertex is the first non-adjacency vertex.
	first,
	
	/// Provoking vertex is the last non-adjacency vertex.
	last
};

} // namespace gl

#endif // ANTKEEPER_GL_PROVOKING_VERTEX_MODE_HPP

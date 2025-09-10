// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <engine/utility/sized-types.hpp>

namespace engine::gl
{
	/// Vertex to be used as the source of data for flat-shaded varyings.
	enum class provoking_vertex_mode: u8
	{
		/// Provoking vertex is the first non-adjacency vertex.
		first,

		/// Provoking vertex is the last non-adjacency vertex.
		last
	};
}

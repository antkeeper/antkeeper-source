// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <engine/gl/vertex-input-rate.hpp>
#include <engine/utility/sized-types.hpp>

namespace engine::gl
{
	/// Vertex input binding.
	struct vertex_input_binding
	{
		/// Binding number that this structure describes.
		u32 binding{0};

		/// Byte stride between consecutive elements within the buffer.
		u32 stride{0};

		/// Specifies whether vertex attribute addressing is a function of the vertex index or of the instance index.
		vertex_input_rate input_rate{vertex_input_rate::vertex};
	};
}

// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <engine/gl/format.hpp>
#include <engine/utility/sized-types.hpp>

namespace engine::gl
{
	/// Vertex input attribute.
	struct vertex_input_attribute
	{
		/// Shader input location number for this attribute.
		u32 location{0};

		/// Binding number which this attribute takes its data from.
		u32 binding{0};

		/// Size and type of the vertex attribute data.
		gl::format format{gl::format::undefined};

		/// Byte offset of this attribute relative to the start of an element in the vertex input binding.
		u32 offset{0};
	};
}

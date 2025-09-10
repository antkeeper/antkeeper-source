// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <engine/gl/vertex-input-attribute.hpp>
#include <engine/gl/vertex-input-binding.hpp>
#include <vector>

namespace engine::gl
{
	/// Pipeline input assembly state.
	struct pipeline_vertex_input_state
	{
		/// Vertex bindings.
		std::vector<vertex_input_binding> vertex_bindings;

		/// Vertex attributes.
		std::vector<vertex_input_attribute> vertex_attributes;
	};
}

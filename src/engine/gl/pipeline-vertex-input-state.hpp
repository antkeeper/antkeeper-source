// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GL_PIPELINE_VERTEX_INPUT_STATE_HPP
#define ANTKEEPER_GL_PIPELINE_VERTEX_INPUT_STATE_HPP

#include <engine/gl/vertex-input-attribute.hpp>
#include <engine/gl/vertex-input-binding.hpp>
#include <vector>

namespace gl {

/// Pipeline input assembly state.
struct pipeline_vertex_input_state
{
	/// Vertex bindings.
	std::vector<vertex_input_binding> vertex_bindings;
	
	/// Vertex attributes.
	std::vector<vertex_input_attribute> vertex_attributes;
};

} // namespace gl

#endif // ANTKEEPER_GL_PIPELINE_VERTEX_INPUT_STATE_HPP

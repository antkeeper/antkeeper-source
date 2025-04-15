// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.gl.pipeline_vertex_input_state;
export import engine.gl.vertex_input_attribute;
export import engine.gl.vertex_input_binding;
export import <vector>;

export namespace engine::gl
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

// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.gl.pipeline_input_assembly_state;
export import engine.gl.primitive_topology;

export namespace engine::gl
{
	/// Pipeline input assembly state.
	struct pipeline_input_assembly_state
	{
		/// Primitive topology.
		primitive_topology topology{primitive_topology::triangle_list};

		/// Controls whether a special vertex index value is treated as restarting the assembly of primitives.
		bool primitive_restart_enabled{false};
	};
}

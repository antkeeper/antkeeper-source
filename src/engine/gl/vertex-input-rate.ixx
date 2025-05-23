// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.gl.vertex_input_rate;
import engine.utility.sized_types;

export namespace engine::gl
{
	/// Rate at which vertex attributes are pulled from buffers.
	enum class vertex_input_rate: u8
	{
		/// Vertex attribute addressing is a function of the vertex index.
		vertex,

		/// Vertex attribute addressing is a function of the instance index.
		instance
	};
}

// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.gl.shader_stage;
import engine.utility.sized_types;

export namespace engine::gl
{
	/// Enumerates all supported shader stages.
	enum class shader_stage: u8
	{
		/// Vertex shader stage.
		vertex,

		/// Fragment shader stage.
		fragment,

		/// Geometry shader stage.
		geometry
	};
}

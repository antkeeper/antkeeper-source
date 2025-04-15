// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.gl.cull_mode;
import engine.utility.sized_types;

export namespace engine::gl
{
	/// Triangle culling mode.
	enum class cull_mode: u8
	{
		/// No triangles are discarded.
		none,

		/// Front-facing triangles are discarded.
		front,

		/// Back-facing triangles are discarded.
		back,

		/// All triangles are discarded.
		front_and_back
	};
}

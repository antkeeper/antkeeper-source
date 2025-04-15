// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.gl.scissor_region;
import engine.utility.sized_types;

export namespace engine::gl
{
	/// Scissor region offset and extents.
	struct scissor_region
	{
		/// X-coordinate offset of the scissor region.
		i32 x{};

		/// Y-coordinate offset of the scissor region.
		i32 y{};

		/// Width of the scissor region.
		u32 width;

		/// Height of the scissor region.
		u32 height;
	};
}

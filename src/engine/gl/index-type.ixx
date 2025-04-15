// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.gl.index_type;
import engine.utility.sized_types;

export namespace engine::gl
{
	/// Index types for index buffers.
	enum class index_type: u8
	{
		/// 8-bit unsigned integer values.
		uint8,

		/// 16-bit unsigned integer values.
		uint16,

		/// 32-bit unsigned integer values.
		uint32
	};
}

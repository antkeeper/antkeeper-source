// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.gl.pixel_type;
import engine.utility.sized_types;

export namespace engine::gl
{
	/// Pixel types.
	enum class pixel_type: u8
	{
		int_8,
		uint_8,
		int_16,
		uint_16,
		int_32,
		uint_32,
		float_16,
		float_32
	};
}

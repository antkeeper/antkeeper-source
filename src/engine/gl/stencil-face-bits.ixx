// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.gl.stencil_face_bits;
import engine.utility.sized_types;

export namespace engine::gl
{
	/// Stencil face mask bits specifying which stencil state(s) to update.
	enum: u8
	{
		/// Only the front set of stencil state is updated.
		stencil_face_front_bit = 0b01,

		/// Only the back set of stencil state is updated.
		stencil_face_back_bit = 0b10,

		/// Both sets of stencil state are updated.
		stencil_face_front_and_back = 0b11,
	};
}

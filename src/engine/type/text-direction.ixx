// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.type.text_direction;

export namespace engine::type
{
	/// Text writing direction.
	enum class text_direction
	{
		/// Text is written from left to right.
		ltr,
	
		/// Text is written from right to left.
		rtl
	};
}

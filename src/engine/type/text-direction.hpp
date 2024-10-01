// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_TYPE_TEXT_DIRECTION_HPP
#define ANTKEEPER_TYPE_TEXT_DIRECTION_HPP

namespace type {

/// Text writing direction.
enum class text_direction
{
	/// Text is written from left to right.
	ltr,
	
	/// Text is written from right to left.
	rtl
};

} // namespace type

#endif // ANTKEEPER_TYPE_TEXT_DIRECTION_HPP

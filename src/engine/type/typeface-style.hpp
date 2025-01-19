// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_TYPE_TYPEFACE_STYLE_HPP
#define ANTKEEPER_TYPE_TYPEFACE_STYLE_HPP

#include <cstdint>

namespace type {

/// Typeface style bitmask bits.
enum typeface_style_bits: std::uint32_t
{
	/// Inidicates a typeface is italic.
	typeface_style_italic_bit = 0b01,
	
	/// Inidicates a typeface is bold.
	typeface_style_bold_bit   = 0b10
};

} // namespace type

#endif // ANTKEEPER_TYPE_TYPEFACE_STYLE_HPP

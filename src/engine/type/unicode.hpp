// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_TYPE_UNICODE_HPP
#define ANTKEEPER_TYPE_UNICODE_HPP

#include <string>
#include <string_view>

namespace type {

/// @name Unicode conversion
/// @{

/// Converts a UTF-8 string to UTF-32.
/// @param utf8 UTF-8 string.
/// @return UTF-32 string.
/// @throw std::runtime_error Invalid UTF-8 sequence.
/// @throw std::runtime_error Invalid UTF-8 byte.
[[nodiscard]] std::u32string to_utf32(std::string_view utf8);

/// Converts a UTF-32 string to UTF-8.
/// @param utf32 UTF-32 string.
/// @return UTF-8 string.
/// @throw std::runtime_error Invalid UTF-32 codepoint.
[[nodiscard]] std::string to_utf8(std::u32string_view utf32);

/// @}

} // namespace type

#endif // ANTKEEPER_TYPE_UNICODE_HPP

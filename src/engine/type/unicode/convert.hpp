// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_TYPE_UNICODE_CONVERT_HPP
#define ANTKEEPER_TYPE_UNICODE_CONVERT_HPP

#include <string>

namespace type {
namespace unicode {

/**
 * Converts a UTF-8 string to UTF-32.
 *
 * @param u8 UTF-8 string.
 * @return UTF-32 string.
 */
[[nodiscard]] std::u32string u32(const std::string& u8);

/**
 * Converts a UTF-32 string to UTF-8.
 *
 * @param u32 UTF-32 string.
 * @return UTF-8 string.
 */
[[nodiscard]] std::string u8(const std::u32string& u32);

} // namespace unicode
} // namespace type

#endif // ANTKEEPER_TYPE_UNICODE_CONVERT_HPP

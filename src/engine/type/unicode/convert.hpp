// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_TYPE_UNICODE_CONVERT_HPP
#define ANTKEEPER_TYPE_UNICODE_CONVERT_HPP

#include <string>
#include <string_view>

namespace type {
namespace unicode {

/**
 * Converts a UTF-8 string to UTF-32.
 *
 * @param view_u8 UTF-8 string view.
 * @return UTF-32 string.
 */
[[nodiscard]] std::u32string u32(std::string_view u8_view);

/**
 * Converts a UTF-32 string to UTF-8.
 *
 * @param view_u32 UTF-32 string view.
 * @return UTF-8 string.
 */
[[nodiscard]] std::string u8(std::u32string_view u32_view);

} // namespace unicode
} // namespace type

#endif // ANTKEEPER_TYPE_UNICODE_CONVERT_HPP

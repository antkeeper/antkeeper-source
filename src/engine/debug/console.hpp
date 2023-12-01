// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_DEBUG_CONSOLE_HPP
#define ANTKEEPER_DEBUG_CONSOLE_HPP

namespace debug {

/**
 * Debug console.
 */
namespace console {

/// Enables VT100 virtual terminal sequences.
void enable_vt100();

/// Disables VT100 virtual terminal sequences.
void disable_vt100();

/// Enables UTF-8 output.
void enable_utf8();

} // namespace console
} // namespace debug

#endif // ANTKEEPER_DEBUG_CONSOLE_HPP

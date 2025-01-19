// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_SCRIPT_IO_MODULE_HPP
#define ANTKEEPER_SCRIPT_IO_MODULE_HPP

#include <engine/script/script-context.hpp>
#include <ostream>

/// Loads the I/O module into a script context.
/// @param ctx Script context.
/// @param cout Standard output stream.
auto load_io_module(script_context& ctx, std::ostream& cout) -> void;

#endif // ANTKEEPER_SCRIPT_IO_MODULE_HPP

// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_SCRIPT_GLOBAL_MODULE_HPP
#define ANTKEEPER_SCRIPT_GLOBAL_MODULE_HPP

#include <engine/script/script-context.hpp>

/// Loads the global module into a script context.
/// @param ctx Script context.
auto load_global_module(script_context& ctx) -> void;

#endif // ANTKEEPER_SCRIPT_GLOBAL_MODULE_HPP

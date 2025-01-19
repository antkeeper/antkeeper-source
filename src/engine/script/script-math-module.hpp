// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_SCRIPT_MATH_MODULE_HPP
#define ANTKEEPER_SCRIPT_MATH_MODULE_HPP

#include <engine/script/script-context.hpp>

/// Loads the math module into a script context.
/// @param ctx Script context.
auto load_math_module(script_context& ctx) -> void;

#endif // ANTKEEPER_SCRIPT_MATH_MODULE_HPP

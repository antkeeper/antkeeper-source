// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <engine/script/script-context.hpp>

namespace engine
{
	/// Loads the math module into a script context.
	/// @param ctx Script context.
	void load_math_module(script_context& ctx);
}

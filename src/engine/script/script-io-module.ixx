// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.script.io_module;
import engine.script.context;
import <ostream>;

export namespace engine
{
	/// Loads the I/O module into a script context.
	/// @param ctx Script context.
	/// @param cout Standard output stream.
	void load_io_module(script_context& ctx, std::ostream& cout);
}

// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_SETTINGS_HPP
#define ANTKEEPER_GAME_SETTINGS_HPP

#include "game/game.hpp"
import engine.debug.log;
import <string_view>;

using namespace engine;

/// Reads a setting if found, inserts a setting if not found, and overwrites a setting if a type mismatch occurs.
/// @tparam T Setting value type.
/// @param[in] ctx Game context.
/// @param[in] key Setting key.
/// @param[in,out] value Setting value.
/// @return `true` if the setting was read, `false` if the setting was written.
template <class T>
bool read_or_write_setting(::game& ctx, std::string_view key, T& value)
{
	if (auto it = ctx.settings->find(key); it != ctx.settings->end())
	{
		try
		{
			it->get_to<T>(value);
		}
		catch (const std::exception& e)
		{
			debug::log_error("Setting type mismatch ({}={}): {}", key, value, e.what());
			return false;
		}
	}
	else
	{
		debug::log_trace("Setting key not found ({}={})", key, value);
		(*ctx.settings)[key] = value;
		return false;
	}
	
	return true;
}

#endif // ANTKEEPER_GAME_SETTINGS_HPP

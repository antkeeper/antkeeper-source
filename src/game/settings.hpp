// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_SETTINGS_HPP
#define ANTKEEPER_GAME_SETTINGS_HPP

#include "game/game.hpp"
#include <engine/debug/log.hpp>


/**
 * Reads a setting if found, inserts a setting if not found, and overwrites a setting if a type mismatch occurs.
 *
 * @tparam T Setting value type.
 *
 * @param[in] ctx Game context.
 * @param[in] key Setting key.
 * @param[in,out] Setting value.
 *
 * @return `true` if the setting was read, `false` if the setting was written.
 */
template <class T>
bool read_or_write_setting(::game& ctx, hash::fnv1a32_t key, T& value)
{
	if (auto i = ctx.settings->find(key); i != ctx.settings->end())
	{
		try
		{
			value = std::any_cast<T>(i->second);
		}
		catch (const std::bad_any_cast&)
		{
			debug::log_error("Setting type mismatch ({:x}={})", key.value, value);
			i->second = value;
			return false;
		}
	}
	else
	{
		debug::log_trace("Setting key not found ({:x}={})", key.value, value);
		(*ctx.settings)[key] = value;
		return false;
	}
	
	return true;
}


#endif // ANTKEEPER_GAME_SETTINGS_HPP

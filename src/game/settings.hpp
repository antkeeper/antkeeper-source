/*
 * Copyright (C) 2023  Christopher J. Howard
 *
 * This file is part of Antkeeper source code.
 *
 * Antkeeper source code is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Antkeeper source code is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Antkeeper source code.  If not, see <http://www.gnu.org/licenses/>.
 */

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
			debug::log::error("Setting type mismatch ({:x}={})", key.value, value);
			i->second = value;
			return false;
		}
	}
	else
	{
		debug::log::trace("Setting key not found ({:x}={})", key.value, value);
		(*ctx.settings)[key] = value;
		return false;
	}
	
	return true;
}


#endif // ANTKEEPER_GAME_SETTINGS_HPP

// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include "game/strings.hpp"
#include <format>

std::string get_string(const ::game& ctx, hash::fnv1a32_t key)
{
	if (auto i = ctx.string_map->find(key); i != ctx.string_map->end())
	{
		return i->second;
	}
	
	return std::format("${:x}", key.value);
}

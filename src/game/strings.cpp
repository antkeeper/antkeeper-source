// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include "game/strings.hpp"
#include <format>

std::string get_string(const ::game& ctx, std::string_view key)
{
	if (auto it = ctx.string_map->find(key); it != ctx.string_map->end())
	{
		if (it->is_string())
		{
			return it->get<std::string>();
		}
	}
	
	return std::format("${}", key);
}

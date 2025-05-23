// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <nlohmann/json.hpp>
#include "game/strings.hpp"
import <format>;

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

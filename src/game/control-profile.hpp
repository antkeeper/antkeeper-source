// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEER_GAME_CONTROL_PROFILE_HPP
#define ANTKEEER_GAME_CONTROL_PROFILE_HPP

import engine.utility.dict;
import engine.hash.fnv;
import engine.input.mapping;
import <map>;
import <memory>;

using namespace engine;

struct control_profile
{
public:
	/// Input mappings.
	std::multimap<hash::fnv32_t, std::unique_ptr<input::mapping>> mappings;
	
	/// Profile-specific settings.
	dict<hash::fnv32_t> settings;
};

#endif // ANTKEEER_GAME_CONTROL_PROFILE_HPP

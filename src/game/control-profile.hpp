// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEER_GAME_CONTROL_PROFILE_HPP
#define ANTKEEER_GAME_CONTROL_PROFILE_HPP

#include <engine/input/mapping.hpp>
#include <engine/utility/dict.hpp>
#include <engine/hash/fnv.hpp>
#include <map>
#include <memory>


struct control_profile
{
public:
	/// Input mappings.
	std::multimap<hash::fnv32_t, std::unique_ptr<input::mapping>> mappings;
	
	/// Profile-specific settings.
	dict<hash::fnv32_t> settings;
};


#endif // ANTKEEER_GAME_CONTROL_PROFILE_HPP

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

#ifndef ANTKEEER_GAME_CONTROL_PROFILE_HPP
#define ANTKEEER_GAME_CONTROL_PROFILE_HPP

#include <engine/input/mapping.hpp>
#include <engine/utility/dict.hpp>
#include <cstdint>
#include <map>
#include <memory>


struct control_profile
{
public:
	/// Input mappings.
	std::multimap<std::uint32_t, std::unique_ptr<input::mapping>> mappings;
	
	/// Profile-specific settings.
	dict<std::uint32_t> settings;
};


#endif // ANTKEEER_GAME_CONTROL_PROFILE_HPP

// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_ANT_CASTE_TYPE_HPP
#define ANTKEEPER_GAME_ANT_CASTE_TYPE_HPP

#include <cstdint>

/// Ant caste types.
/// @see https://www.antwiki.org/wiki/Caste_Terminology
enum class ant_caste_type: std::uint8_t
{
	/// Queen caste.
	queen   = 0b0001,
	
	/// Worker caste.
	worker  = 0b0010,
	
	/// Soldier caste.
	soldier = 0b0100,
	
	/// Male caste.
	male    = 0b1000
};

#endif // ANTKEEPER_GAME_ANT_CASTE_TYPE_HPP

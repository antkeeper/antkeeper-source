// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_ANT_SUBCASTE_HPP
#define ANTKEEPER_GAME_ANT_SUBCASTE_HPP

import engine.utility.sized_types;

/// Ant subcaste types.
enum class ant_subcaste: u8
{
	/// Worker from the queen's first batch of eggs, smaller than normal workers.
	nanitic = 1,
	
	/// Small worker or soldier.
	minor,
	
	/// Normal-sized worker or soldier
	media,
	
	/// Large worker or soldier.
	major,
	
	/// Winged queen or male.
	alate,
	
	/// Dewinged queen or male.
	dealate,
	
	/// Queen or male which does not develop wings.
	ergatoid,
	
	/// Queen or male with short, nonfunctional wings.
	brachypterous
};

#endif // ANTKEEPER_GAME_ANT_SUBCASTE_HPP

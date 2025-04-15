// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_NEST_COMPONENT_HPP
#define ANTKEEPER_GAME_NEST_COMPONENT_HPP

import engine.entity.id;
import <vector>;

using namespace engine;

struct nest_component
{
	std::vector<entity::id> chambers;
	float helix_radius;
	float helix_pitch;
	float helix_chirality;
	float helix_turns;
};

#endif // ANTKEEPER_GAME_NEST_COMPONENT_HPP

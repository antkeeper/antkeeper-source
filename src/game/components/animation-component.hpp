// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_ANIMATION_COMPONENT_HPP
#define ANTKEEPER_GAME_ANIMATION_COMPONENT_HPP

import engine.animation.animation_player;

using namespace engine;

struct animation_component
{
	animation::animation_player player;
};

#endif // ANTKEEPER_GAME_ANIMATION_COMPONENT_HPP

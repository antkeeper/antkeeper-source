// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_ANT_SWARM_HPP
#define ANTKEEPER_GAME_ANT_SWARM_HPP

#include "game/game.hpp"
#include <engine/entity/id.hpp>

entity::id create_ant_swarm(::game& ctx);
void destroy_ant_swarm(::game& ctx, entity::id swarm_eid);

#endif // ANTKEEPER_GAME_ANT_SWARM_HPP

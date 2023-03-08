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

#ifndef ANTKEEPER_GAME_SPAWN_HPP
#define ANTKEEPER_GAME_SPAWN_HPP

#include "game/game.hpp"
#include "game/ant/ant-genome.hpp"
#include <engine/utility/fundamental-types.hpp>


/**
 * Spawns an ant egg.
 *
 * @param ctx Game context.
 * @param genome Ant genome.
 * @param fertilized Whether the egg has been fertilized.
 * @param position Position at which to spawn an egg.
 *
 * @return Entity ID of the spawned ant egg.
 */
entity::id spawn_ant_egg(::game& ctx, const ant_genome& genome, bool fertilized, const float3& position);

/**
 * Spawns an ant larva.
 *
 * @param ctx Game context.
 * @param genome Ant genome.
 * @param position Position at which to spawn an larva.
 *
 * @return Entity ID of the spawned ant larva.
 */
entity::id spawn_ant_larva(::game& ctx, const ant_genome& genome, const float3& position);

/**
 * Spawns a worker ant.
 *
 * @param ctx Game context.
 * @param genome Ant genome.
 * @param position Position at which to spawn a worker ant.
 *
 * @return Entity ID of the spawned worker ant.
 */
entity::id spawn_worker_ant(::game& ctx, const ant_genome& genome, const float3& position);


#endif // ANTKEEPER_GAME_SPAWN_HPP

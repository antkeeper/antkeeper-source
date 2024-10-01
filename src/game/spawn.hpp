// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_SPAWN_HPP
#define ANTKEEPER_GAME_SPAWN_HPP

#include "game/game.hpp"
#include "game/ant/ant-genome.hpp"
#include <engine/math/vector.hpp>


/// Spawns an ant egg.
/// @param ctx Game context.
/// @param genome Ant genome.
/// @param position Position at which to spawn an egg.
/// @return Entity ID of the spawned ant egg.
entity::id spawn_ant_egg(::game& ctx, const ant_genome& genome, const math::fvec3& position);

/// Spawns an ant larva.
/// @param ctx Game context.
/// @param genome Ant genome.
/// @param position Position at which to spawn an larva.
/// @return Entity ID of the spawned ant larva.
entity::id spawn_ant_larva(::game& ctx, const ant_genome& genome, const math::fvec3& position);

// 
// Spawns a worker ant.
// 
// @param ctx Game context.
// @param genome Ant genome.
// @param position Position at which to spawn a worker ant.
// 
// @return Entity ID of the spawned worker ant.
// 
// entity::id spawn_worker_ant(::game& ctx, const ant_genome& genome, const math::fvec3& position);

#endif // ANTKEEPER_GAME_SPAWN_HPP

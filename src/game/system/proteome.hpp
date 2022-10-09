/*
 * Copyright (C) 2021  Christopher J. Howard
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

#ifndef ANTKEEPER_GAME_SYSTEM_PROTEOME_HPP
#define ANTKEEPER_GAME_SYSTEM_PROTEOME_HPP

#include "game/system/updatable.hpp"
#include "game/component/genome.hpp"
#include "entity/id.hpp"

namespace game {
namespace system {

/**
 * Generates proteomes for every genome.
 */
class proteome:
	public updatable
{
public:
	proteome(entity::registry& registry);
	~proteome();
	
	/**
	 * Scales then adds the timestep `dt` to the current time, then recalculates the positions of proteomeing bodies.
	 *
	 * @param t Time, in seconds.
	 * @param dt Delta time, in seconds.
	 */
	virtual void update(double t, double dt);
	
private:
	void on_genome_construct(entity::registry& registry, entity::id entity_id);
	void on_genome_update(entity::registry& registry, entity::id entity_id);
};

} // namespace system
} // namespace game

#endif // ANTKEEPER_GAME_SYSTEM_PROTEOME_HPP

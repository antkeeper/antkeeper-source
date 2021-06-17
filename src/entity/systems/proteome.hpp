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

#ifndef ANTKEEPER_ENTITY_SYSTEM_PROTEOME_HPP
#define ANTKEEPER_ENTITY_SYSTEM_PROTEOME_HPP

#include "entity/systems/updatable.hpp"
#include "entity/components/genome.hpp"
#include "entity/id.hpp"

namespace entity {
namespace system {

/**
 * Generates proteomes for every genome.
 */
class proteome:
	public updatable
{
public:
	proteome(entity::registry& registry);
	
	/**
	 * Scales then adds the timestep `dt` to the current time, then recalculates the positions of proteomeing bodies.
	 *
	 * @param t Time, in seconds.
	 * @param dt Delta time, in seconds.
	 */
	virtual void update(double t, double dt);
	
private:
	void on_genome_construct(entity::registry& registry, entity::id entity_id, entity::component::genome& genome);
	void on_genome_replace(entity::registry& registry, entity::id entity_id, entity::component::genome& genome);
};

} // namespace system
} // namespace entity

#endif // ANTKEEPER_ENTITY_SYSTEM_PROTEOME_HPP

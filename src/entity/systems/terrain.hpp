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

#ifndef ANTKEEPER_ENTITY_SYSTEM_TERRAIN_HPP
#define ANTKEEPER_ENTITY_SYSTEM_TERRAIN_HPP

#include "entity/systems/updatable.hpp"
#include "entity/components/terrain.hpp"
#include "entity/id.hpp"

namespace entity {
namespace system {

class terrain: public updatable
{
public:
	terrain(entity::registry& registry);
	~terrain();
	
	virtual void update(double t, double dt);
	
	/**
	 * Sets the number of subdivisions for a patch.
	 *
	 * @param n Number of subdivisions.
	 */
	void set_patch_subdivisions(std::uint8_t n);

private:
	void on_terrain_construct(entity::registry& registry, entity::id entity_id, entity::component::terrain& component);
	void on_terrain_destroy(entity::registry& registry, entity::id entity_id);
	
	void generate_patch();
	
	std::uint8_t patch_subdivisions;
	std::size_t patch_vertex_size;
	std::size_t patch_vertex_count;
	float* patch_vertex_data;
};

} // namespace system
} // namespace entity

#endif // ANTKEEPER_ENTITY_SYSTEM_TERRAIN_HPP

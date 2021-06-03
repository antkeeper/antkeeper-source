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
#include "geom/mesh.hpp"

class terrain;
class resource_manager;
class model;
class image;

namespace entity {
namespace system {

class terrain: public updatable
{
public:
	terrain(entity::registry& registry, ::resource_manager* resource_manager);
	~terrain();
	virtual void update(double t, double dt);
	
	/**
	 * Sets the size of a single terrain patch.
	 */
	void set_patch_size(float size);

private:
	geom::mesh* generate_terrain_mesh(float size, int subdivisions);
	model* generate_terrain_model(geom::mesh* terrain_mesh);
	void project_terrain_mesh(geom::mesh* terrain_mesh, const entity::component::terrain& component);
	void update_terrain_model(model* terrain_model, geom::mesh* terrain_mesh);

	void on_terrain_construct(entity::registry& registry, entity::id entity_id, entity::component::terrain& component);
	void on_terrain_destroy(entity::registry& registry, entity::id entity_id);

	resource_manager* resource_manager;
	float patch_size;
	float heightmap_size;
	float heightmap_scale;
	image* heightmap;
};

} // namespace system
} // namespace entity

#endif // ANTKEEPER_ENTITY_SYSTEM_TERRAIN_HPP


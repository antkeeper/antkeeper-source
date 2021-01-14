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

#ifndef ANTKEEPER_ECS_TERRAIN_SYSTEM_HPP
#define ANTKEEPER_ECS_TERRAIN_SYSTEM_HPP

#include "entity-system.hpp"
#include "ecs/components/terrain-component.hpp"
#include "ecs/entity.hpp"
#include "geom/mesh.hpp"

class terrain;
class resource_manager;
class model;
class image;

namespace ecs {

class terrain_system: public entity_system
{
public:
	terrain_system(ecs::registry& registry, ::resource_manager* resource_manager);
	~terrain_system();
	virtual void update(double t, double dt);
	
	/**
	 * Sets the size of a single terrain patch.
	 */
	void set_patch_size(float size);

private:
	geom::mesh* generate_terrain_mesh(float size, int subdivisions);
	model* generate_terrain_model(geom::mesh* terrain_mesh);
	void project_terrain_mesh(geom::mesh* terrain_mesh, const ecs::terrain_component& component);
	void update_terrain_model(model* terrain_model, geom::mesh* terrain_mesh);

	void on_terrain_construct(ecs::registry& registry, ecs::entity entity, ecs::terrain_component& component);
	void on_terrain_destroy(ecs::registry& registry, ecs::entity entity);

	resource_manager* resource_manager;
	float patch_size;
	float heightmap_size;
	float heightmap_scale;
	image* heightmap;
};

} // namespace ecs

#endif // ANTKEEPER_ECS_TERRAIN_SYSTEM_HPP


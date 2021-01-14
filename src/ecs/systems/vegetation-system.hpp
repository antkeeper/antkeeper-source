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

#ifndef ANTKEEPER_ECS_VEGETATION_SYSTEM_HPP
#define ANTKEEPER_ECS_VEGETATION_SYSTEM_HPP

#include "entity-system.hpp"
#include "ecs/components/terrain-component.hpp"
#include "ecs/entity.hpp"
#include "scene/collection.hpp"

class model;

namespace ecs {

/**
 * Places vegetation patches on terrain.
 */
class vegetation_system: public entity_system
{
public:
	vegetation_system(ecs::registry& registry);
	~vegetation_system();
	virtual void update(double t, double dt);
	
	/**
	 * Sets the terrain patch size.
	 *
	 * @param size Size of the terrain patch.
	 */
	void set_terrain_patch_size(float size);
	
	/**
	 * Sets the vegetation patch size.
	 *
	 * @param subdivisions Number of times a terrain patch should be subdivided into vegetation patches.
	 */
	void set_vegetation_patch_resolution(int subdivisions);
	
	void set_vegetation_density(float density);
	
	void set_vegetation_model(::model* model);
	
	void set_scene(scene::collection* collection);

private:
	void on_terrain_construct(ecs::registry& registry, ecs::entity entity, ecs::terrain_component& component);
	void on_terrain_destroy(ecs::registry& registry, ecs::entity entity);

	float terrain_patch_size;
	float vegetation_patch_size;
	int vegetation_patch_columns;
	int vegetation_patch_rows;
	float vegetation_density;
	model* vegetation_model;
	scene::collection* scene_collection;
};

} // namespace ecs

#endif // ANTKEEPER_ECS_VEGETATION_SYSTEM_HPP


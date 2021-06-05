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

#include "vegetation.hpp"
#include "entity/components/model.hpp"
#include "entity/components/transform.hpp"
#include "scene/model-instance.hpp"
#include "scene/lod-group.hpp"
#include "scene/collection.hpp"
#include "renderer/material.hpp"
#include "geom/aabb.hpp"
#include "utility/fundamental-types.hpp"
#include <cmath>

namespace entity {
namespace system {

vegetation::vegetation(entity::registry& registry):
	updatable(registry),
	terrain_patch_size(1.0f),
	vegetation_patch_size(1.0f),
	vegetation_patch_columns(1),
	vegetation_patch_rows(1),
	vegetation_density(1.0f),
	vegetation_model(nullptr)
{
	registry.on_construct<component::terrain>().connect<&vegetation::on_terrain_construct>(this);
	registry.on_destroy<component::terrain>().connect<&vegetation::on_terrain_destroy>(this);
}

vegetation::~vegetation()
{}

void vegetation::update(double t, double dt)
{}

void vegetation::set_terrain_patch_size(float size)
{
	terrain_patch_size = size;
	vegetation_patch_size = terrain_patch_size / static_cast<float>(vegetation_patch_columns);
}

void vegetation::set_vegetation_patch_resolution(int subdivisions)
{
	// Determine number of vegetation patch columns and rows per terrain patch
	vegetation_patch_columns = static_cast<int>(std::pow(2, subdivisions));
	vegetation_patch_rows = vegetation_patch_columns;
	vegetation_patch_size = terrain_patch_size / static_cast<float>(vegetation_patch_columns);
}

void vegetation::set_vegetation_density(float density)
{
	vegetation_density = density;
}

void vegetation::set_vegetation_model(::model* model)
{
	vegetation_model = model;
}

void vegetation::set_scene(scene::collection* collection)
{
	this->scene_collection = collection;
}

void vegetation::on_terrain_construct(entity::registry& registry, entity::id entity_id, component::terrain& component)
{}

void vegetation::on_terrain_destroy(entity::registry& registry, entity::id entity_id)
{}

} // namespace system
} // namespace entity

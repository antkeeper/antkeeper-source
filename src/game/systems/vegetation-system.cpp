/*
 * Copyright (C) 2020  Christopher J. Howard
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

#include "vegetation-system.hpp"
#include "game/components/model-component.hpp"
#include "game/components/transform-component.hpp"
#include "scene/model-instance.hpp"
#include "scene/lod-group.hpp"
#include "scene/scene.hpp"
#include "renderer/material.hpp"
#include "geometry/aabb.hpp"
#include "utility/fundamental-types.hpp"
#include <cmath>

using namespace ecs;

vegetation_system::vegetation_system(entt::registry& registry):
	entity_system(registry),
	terrain_patch_size(1.0f),
	vegetation_patch_size(1.0f),
	vegetation_patch_columns(1),
	vegetation_patch_rows(1),
	vegetation_density(1.0f),
	vegetation_model(nullptr)
{
	registry.on_construct<terrain_component>().connect<&vegetation_system::on_terrain_construct>(this);
	registry.on_destroy<terrain_component>().connect<&vegetation_system::on_terrain_destroy>(this);
}

vegetation_system::~vegetation_system()
{}

void vegetation_system::update(double t, double dt)
{}

void vegetation_system::set_terrain_patch_size(float size)
{
	terrain_patch_size = size;
	vegetation_patch_size = terrain_patch_size / static_cast<float>(vegetation_patch_columns);
}

void vegetation_system::set_vegetation_patch_resolution(int subdivisions)
{
	// Determine number of vegetation patch columns and rows per terrain patch
	vegetation_patch_columns = static_cast<int>(std::pow(2, subdivisions));
	vegetation_patch_rows = vegetation_patch_columns;
	vegetation_patch_size = terrain_patch_size / static_cast<float>(vegetation_patch_columns);
}

void vegetation_system::set_vegetation_density(float density)
{
	vegetation_density = density;
}

void vegetation_system::set_vegetation_model(::model* model)
{
	vegetation_model = model;
}

void vegetation_system::set_scene(::scene* scene)
{
	this->scene = scene;
}

void vegetation_system::on_terrain_construct(entt::registry& registry, entt::entity entity, terrain_component& component)
{
	// Find corner of terrain patch
	float terrain_patch_min_x = static_cast<float>(component.x) * terrain_patch_size - terrain_patch_size * 0.5f;
	float terrain_patch_min_z = static_cast<float>(component.z) * terrain_patch_size - terrain_patch_size * 0.5f;
	
	// Create vegetation patches
	for (int column = 0; column < vegetation_patch_columns; ++column)
	{
		for (int row = 0; row < vegetation_patch_rows; ++row)
		{
			// Calculate center of vegetation patch

			
			/*
			// Create vegetation patch entity
			auto vegetation_patch_entity = registry.create();
			
			// Assign a transform component
			transform_component transform;
			transform.transform = math::identity_transform<float>;
			transform.transform.translation = float3{vegetation_patch_x, 0.0f, vegetation_patch_z};
			transform.warp = true;
			registry.assign_or_replace<transform_component>(vegetation_patch_entity, transform);
			
			// Assign a model component
			model_component model;
			model.model = vegetation_model;
			model.instance_count = 500;
			registry.assign_or_replace<model_component>(vegetation_patch_entity, model);
			*/
			
			// Find patch translation
			float vegetation_patch_x = terrain_patch_min_x + vegetation_patch_size * static_cast<float>(column) + vegetation_patch_size * 0.5f;
			float vegetation_patch_z = terrain_patch_min_z + vegetation_patch_size * static_cast<float>(row) + vegetation_patch_size * 0.5f;
			float3 translation = {vegetation_patch_x, 0.0f, vegetation_patch_z};
			
			// Generate culling mask
			aabb<float>* culling_mask = new aabb<float>(vegetation_model->get_bounds());
			culling_mask->min_point.x = std::min<float>(culling_mask->min_point.x, translation.x - vegetation_patch_size * 0.5f);
			culling_mask->min_point.z = std::min<float>(culling_mask->min_point.z, translation.z - vegetation_patch_size * 0.5f);
			culling_mask->max_point.x = std::max<float>(culling_mask->max_point.x, translation.x + vegetation_patch_size * 0.5f);
			culling_mask->max_point.z = std::max<float>(culling_mask->max_point.z, translation.z + vegetation_patch_size * 0.5f);
			
			std::size_t lod_count = 4;
			std::size_t instance_count_lod0 = 500;
			std::size_t instance_count_lod1 = instance_count_lod0 / 2;
			std::size_t instance_count_lod2 = instance_count_lod1 / 2;
			
			// Generate LOD materials
			const material* lod0_material = (*vegetation_model->get_groups())[0]->get_material();
			material* lod1_material = new material(*lod0_material);
			static_cast<material_property<int>*>(lod1_material->get_property("instance_multiplier"))->set_value(2);
			material* lod2_material = new material(*lod0_material);
			static_cast<material_property<int>*>(lod2_material->get_property("instance_multiplier"))->set_value(4);
			
			// Create LOD 0
			model_instance* patch_lod0 = new model_instance();
			patch_lod0->set_model(vegetation_model);
			patch_lod0->set_translation(translation);
			patch_lod0->set_instanced(true, instance_count_lod0);
			patch_lod0->set_culling_mask(culling_mask);
			patch_lod0->update_tweens();
			
			// Create LOD 1
			model_instance* patch_lod1 = new model_instance();
			patch_lod1->set_model(vegetation_model);
			patch_lod1->set_material(0, lod1_material);
			patch_lod1->set_translation(translation);
			patch_lod1->set_instanced(true, instance_count_lod1);
			patch_lod1->set_culling_mask(culling_mask);
			patch_lod1->update_tweens();
			
			// Create LOD 2
			model_instance* patch_lod2 = new model_instance();
			patch_lod2->set_model(vegetation_model);
			patch_lod2->set_material(0, lod2_material);
			patch_lod2->set_translation(translation);
			patch_lod2->set_instanced(true, instance_count_lod2);
			patch_lod2->set_culling_mask(culling_mask);
			patch_lod2->update_tweens();
			
			// Create LOD group
			::lod_group* lod_group = new ::lod_group(lod_count);
			lod_group->add_object(0, patch_lod0);
			lod_group->add_object(1, patch_lod1);
			lod_group->add_object(2, patch_lod2);
			lod_group->set_translation(translation);
			lod_group->update_tweens();
			
			// Add LOD group to scene
			//scene->add_object(lod_group);
		}
	}
}

void vegetation_system::on_terrain_destroy(entt::registry& registry, entt::entity entity)
{}

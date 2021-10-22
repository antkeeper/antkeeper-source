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

#include "scene/model-instance.hpp"
#include "render/model.hpp"
#include "render/material.hpp"

namespace scene {

model_instance::model_instance(render::model* model):
	model(nullptr),
	pose(nullptr),
	local_bounds{{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}},
	world_bounds{{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}},
	instanced(false),
	instance_count(0)
{
	set_model(model);
	update_bounds();
}

model_instance::model_instance():
	model_instance(nullptr)
{}

model_instance::model_instance(const model_instance& other)
{
	*this = other;
}

model_instance& model_instance::operator=(const model_instance& other)
{
	local_bounds = other.local_bounds;
	world_bounds = other.world_bounds;
	model = other.model;
	pose = other.pose;
	materials = other.materials;
	instanced = other.instanced;
	instance_count = other.instance_count;
	return *this;
}

void model_instance::set_model(render::model* model)
{
	this->model = model;
	this->pose = nullptr;

	if (model)
	{
		materials.resize(model->get_groups()->size());
		reset_materials();
	}
	
	update_bounds();
}

void model_instance::set_pose(::pose* pose)
{
	this->pose = pose;
}

void model_instance::set_material(std::size_t group_index, render::material* material)
{
	materials[group_index] = material;
}

void model_instance::set_instanced(bool instanced, std::size_t instance_count)
{
	this->instanced = instanced;
	this->instance_count = (instanced) ? instance_count : 0;
}

void model_instance::reset_materials()
{
	std::fill(materials.begin(), materials.end(), nullptr);
}

void model_instance::update_bounds()
{
	if (model)
	{
		local_bounds = aabb_type::transform(model->get_bounds(), get_transform());
		transformed();
	}
	else
	{
		local_bounds = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};
		world_bounds = {get_translation(), get_translation()};
	}
}

void model_instance::transformed()
{
	world_bounds = aabb_type::transform(local_bounds, get_transform());
}

void model_instance::update_tweens()
{
	object_base::update_tweens();
	
	// Update model material tweens
	if (model)
	{
		for (render::model_group* group: *model->get_groups())
		{
			render::material* material = group->get_material();
			if (material)
			{
				material->update_tweens();
			}
		}
	}
	
	// Update material override tweens
	for (render::material* material: materials)
	{
		if (material)
		{
			material->update_tweens();
		}
	}
}

} // namespace scene
